#pragma once

#include "../BuildingStructsActor.h"

class BuildingFlowerHomes {
public:
	BuildingFlowerHomes();
	~BuildingFlowerHomes();

	static std::tuple<FBuildingBlueprint, TArray<FVector>> Create(TArray<FVector> pathVertices, float centerAreaRatio = 0.15,
		float homeLandSizeSqM = 2000.0, FString homeLocation = "outer", int hexesPerHome = 3,
		int floorsPerHome = 1, int maxFloors = 1, FString uName = "", float unitDiameter = 9.0, float floorHeight = 3.5,
		int maxUnitsRoof = 5, int crossCountUnits = 2);
};
