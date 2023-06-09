#pragma once

#include "../BuildingStructsActor.h"

class PlotBuild {
public:
	PlotBuild();
	~PlotBuild();

	static TArray<FLand> CreateLands(TMap<FString, FPlot> plots);
	static void DrawLands(TArray<FLand> lands);
};
