#pragma once

#include <mutex>

#include "../BuildingStructsActor.h"

class PlotData {
private:
	static PlotData *pinstance_;
	static std::mutex mutex_;

	TMap<FString, FPlot> _plots = {};
public:
	PlotData();
	~PlotData();

	// Singletons should not be cloneable or assignable.
	PlotData(PlotData &other) = delete;
	void operator=(const PlotData &) = delete;
	static PlotData *GetInstance();

	void CleanUp();
	void SavePlotsToFile();
	void DeletePlot(FString uName, bool deleteChildren = true);
	void SavePlotFromPolygon(FString uName, FPolygon polygon);
	TMap<FString, FPlot> LoadPlots();
	TMap<FString, FPlot> LoadAndSubdividePlots(bool removeFinalChildren = false);
	int CheckSubdividePlots();
	FString GetParentPattern(FString childUName);
	void RemoveChildren(bool save = true);
};
