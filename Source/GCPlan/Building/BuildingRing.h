#pragma once

#include "../BuildingStructsActor.h"

class BuildingRing {
public:
	BuildingRing();
	~BuildingRing();

	static FBuildingBlueprint Create(TArray<FVector> pathVertices, TArray<int> heightFloorsOrder,
	    int residentCount = 1000, int minRingWidthUnits = 1, int maxRingWidthUnits = 5,
	    float unitDiameter = 9, float unitHeight = 3.5,
	    int numRings = 1, float residentToUnitCountFactor = 2.7,
	    FString crossDirection = "outToIn", bool placeOnSides = false, float maxBuiltRatio = 0.55,
	    FString uName = "");
	static FBuildingBlueprint CreateOneRing(int unitCount,
		TArray<FVector> pathVertices, TArray<int> heightFloorsOrder,
		int minRingWidthUnits = 1, int maxRingWidthUnits = 5, float unitDiameter = 9,
		float unitHeight = 3.5, FString crossDirection = "outToIn", bool placeOnSides = false,
		float maxBuiltRatio = 0.55, FString uName = "");
	static std::tuple<int, float> GetMaxRingsByAreaOuter(float radius, float unitDiameter, int maxRings,
    	float maxBuiltRatio = 0.55);
};
