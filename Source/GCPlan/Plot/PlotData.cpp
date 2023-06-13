#include "PlotData.h"

#include "PlotDivide.h"
#include "../BuildingStructsActor.h"
#include "../Common/UnrealGlobal.h"
#include "../Data/DataFileProject.h"

PlotData* PlotData::pinstance_{nullptr};
std::mutex PlotData::mutex_;

PlotData::PlotData() {
}

PlotData::~PlotData() {
}

PlotData *PlotData::GetInstance() {
	std::lock_guard<std::mutex> lock(mutex_);
	if (pinstance_ == nullptr) {
		pinstance_ = new PlotData();
	}
	return pinstance_;
}

void PlotData::CleanUp() {
	_plots.Empty();
}

void PlotData::SavePlotsToFile() {
	UnrealGlobal* unrealGlobal = UnrealGlobal::GetInstance();
	DataFileProject* dataFileProject = DataFileProject::GetInstance();
	FDataProjectJson* json = new FDataProjectJson(_plots);
	dataFileProject->SaveProject(*json, unrealGlobal->_settings->jsonFiles["plot"]);
}

void PlotData::DeletePlot(FString uName, bool deleteChildren) {
	FString parent;
	if (_plots.Contains(uName)) {
		// Remove from associated plots too.
		parent = _plots[uName].parentPlotUName;
		if (parent.Len() && _plots.Contains(parent) &&
			_plots[parent].childPlotUNames.Contains(uName)) {
			_plots[parent].childPlotUNames.Remove(uName);
		}
		if (deleteChildren && _plots[uName].childPlotUNames.Num() > 0) {
			for (int ii = 0; ii < _plots[uName].childPlotUNames.Num(); ii++) {
				DeletePlot(_plots[uName].childPlotUNames[ii], true);
			}
		}
		_plots.Remove(uName);
		SavePlotsToFile();
	}
}

void PlotData::SavePlotFromPolygon(FString uName, FPolygon polygon) {
	if (!_plots.Contains(uName)) {
		_plots.Add(uName, ABuildingStructsActor::PolygonToPlot(polygon));
	}
	_plots[uName] = ABuildingStructsActor::UpdatePlotFromPolygon(polygon, _plots[uName]);
	SavePlotsToFile();
}

TMap<FString, FPlot> PlotData::LoadPlots() {
	_plots.Empty();
	UnrealGlobal* unrealGlobal = UnrealGlobal::GetInstance();
	DataFileProject* dataFileProject = DataFileProject::GetInstance();
	auto [data, valid] = dataFileProject->LoadProject(unrealGlobal->_settings->jsonFiles["plot"]);
	if (valid) {
		_plots = data.plots;
	}
	return _plots;
}

TMap<FString, FPlot> PlotData::LoadAndSubdividePlots(bool removeFinalChildren) {
	_plots.Empty();
	UnrealGlobal* unrealGlobal = UnrealGlobal::GetInstance();
	DataFileProject* dataFileProject = DataFileProject::GetInstance();
	auto [data, valid] = dataFileProject->LoadProject(unrealGlobal->_settings->jsonFiles["plot"]);
	if (valid) {
		// if (removeFinalChildren) {
		// 	for (auto& Elem : data.plots) {
		// 		if (Elem.Value.childPlotUNames.Num() > 0) {
		// 			_plots.Add(Elem.Key, Elem.Value);
		// 		}
		// 	}
		// } else {
		// 	_plots = data.plots;
		// }
		_plots = data.plots;
		if (removeFinalChildren) {
			RemoveChildren(false);
		}
	}

	int countNew = CheckSubdividePlots();
	// auto [plots1, countNew] = PlotDivide::SubdividePlots(_plots);
	if (countNew > 0) {
		UE_LOG(LogTemp, Warning, TEXT("PlotData.LoadAndSubdividePlots saving %d new plots"), countNew);
		// _plots = plots1;
		SavePlotsToFile();
	}

	PlotDivide::AddRoads(_plots);

	return _plots;
}

int PlotData::CheckSubdividePlots() {
	int countNew = 0;
	TMap<FString, FPlot> newPlots = {};
	for (auto& Elem : _plots) {
		if (Elem.Value.parentPlotUName == "" && Elem.Value.childPlotUNames.Num() < 1) {
			auto [plots1, countNew1] = PlotDivide::SubdividePlots({ { Elem.Key, Elem.Value} });
			_plots[Elem.Key] = plots1[Elem.Key];
			for (auto& Elem1 : plots1) {
				// Can not update plots directly since it we are looping through them.
				newPlots.Add(Elem1.Key, Elem1.Value);
				countNew += 1;
			}
		}
	}
	for (auto& Elem : newPlots) {
		_plots.Add(Elem.Key, Elem.Value);
	}
	return countNew;
}

FString PlotData::GetParentPattern(FString childUName) {
	FString buildPattern = "";
	if (_plots.Contains(childUName)) {
		FString uName = _plots[childUName].parentPlotUName;
		while (uName != "" && _plots.Contains(uName)) {
			if (_plots[uName].buildPattern != "") {
				return _plots[uName].buildPattern;
			}
		}
	}
	return buildPattern;
}

void PlotData::RemoveChildren(bool save) {
	TMap<FString, FPlot> plotsToKeep = {};
	for (auto& Elem : _plots) {
		if (Elem.Value.parentPlotUName == "") {
			// Empty out children.
			Elem.Value.childPlotUNames.Empty();
			plotsToKeep.Add(Elem.Key, Elem.Value);
		}
	}
	_plots = plotsToKeep;
	if (save) {
		SavePlotsToFile();
	}
}
