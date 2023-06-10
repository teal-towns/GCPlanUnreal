#pragma once

#include "../BuildingStructsActor.h"

class LandNature {

public:
	LandNature();
	~LandNature();

	static void PlaceNature(TMap<FString, FPlot> plots);
};
