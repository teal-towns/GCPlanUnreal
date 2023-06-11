#pragma once

#include "../BuildingStructsActor.h"

class PlotDivide {
public:
	PlotDivide();
	~PlotDivide();

	static std::tuple<TMap<FString, FPlot>, int> SubdividePlots(TMap<FString, FPlot> Plots,
		float minRadiusSkip = 15, float minSquareMetersSkip = 2000, bool addRoads = false);
	static bool AddRoads(TMap<FString, FPlot> Plots);
};
