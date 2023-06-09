#pragma once

#include "../BuildingStructsActor.h"

class PlotDivide {
public:
	PlotDivide();
	~PlotDivide();

	static TMap<FString, FPlot> SubdividePlots(TMap<FString, FPlot> Plots, float minRadiusSkip = 15,
		float minSquareMetersSkip = 2000, bool addRoads = true);
};
