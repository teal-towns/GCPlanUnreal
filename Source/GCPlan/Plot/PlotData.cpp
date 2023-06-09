#include "PlotData.h"

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


