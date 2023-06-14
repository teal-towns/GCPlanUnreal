#pragma once

#include "../BuildingStructsActor.h"

class PlotDivide {
public:
	PlotDivide();
	~PlotDivide();

	static std::tuple<TMap<FString, FPolygon>, int> SubdividePlots(TMap<FString, FPolygon> polygons,
		float minRadiusSkip = 15, float minSquareMetersSkip = 2000, bool addRoads = false);
	static bool AddRoads(TMap<FString, FPolygon> polygons);
};
