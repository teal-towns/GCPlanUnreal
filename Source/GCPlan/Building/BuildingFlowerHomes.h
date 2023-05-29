#pragma once

#include "../BuildingStructsActor.h"

class BuildingFlowerHomes {
public:
	BuildingFlowerHomes();
	~BuildingFlowerHomes();

	static std::tuple<FBuildingBlueprint, TMap<FString, FRoadPath>> Create(TArray<FVector> pathVertices,
		float verticesBuffer = -25, float centerAreaRatio = 0.15,
		float homeLandSizeSqM = 2000.0, FString homeLocation = "outer",
		int maxFloors = 2, FString uName = "", float unitDiameter = 9.0, float floorHeight = 3.5,
		int maxUnitsRoof = 3, int crossCountUnits = 2, float minHomeLandSizeSqM = 250);
	static FBuildingBlueprint BuildColumn(FBuildingBlueprint blueprint, FVector posCurrentGround, int numFloors,
		float floorHeight);
	static FBuildingBlueprint BuildHexHome(FBuildingBlueprint blueprint, TArray<FVector> pathVertices, FVector innerPoint,
		int numRoofUnits, int numFloors, float unitDiameter, float floorHeight);
};
