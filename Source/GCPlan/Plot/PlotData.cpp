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
	FDataProjectJson* json = new FDataProjectJson(_plots);
	DataFileProject::SaveProject(*json, unrealGlobal->_settings->projectJsonFiles["plot"]);
}

void PlotData::DeletePlot(FString uName) {
	if (_plots.Contains(uName)) {
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
	auto [data, valid] = DataFileProject::LoadProject(unrealGlobal->_settings->projectJsonFiles["plot"]);
	if (valid) {
		_plots = data.plots;
	}
	return _plots;
}

TMap<FString, FPlot> PlotData::LoadAndSubdividePlots(bool removeFinalChildren) {
	_plots.Empty();
	UnrealGlobal* unrealGlobal = UnrealGlobal::GetInstance();
	auto [data, valid] = DataFileProject::LoadProject(unrealGlobal->_settings->projectJsonFiles["plot"]);
	if (valid) {
		if (removeFinalChildren) {
			for (auto& Elem : data.plots) {
				if (Elem.Value.childPlotUNames.Num() > 0) {
					_plots.Add(Elem.Key, Elem.Value);
				}
			}
		} else {
			_plots = data.plots;
		}
	}

	auto [plots1, countNew] = PlotDivide::SubdividePlots(_plots);
	if (countNew > 0) {
		_plots = plots1;
		FDataProjectJson* json = new FDataProjectJson(_plots);
		DataFileProject::SaveProject(*json, unrealGlobal->_settings->projectJsonFiles["plot"]);
	}

	return _plots;
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