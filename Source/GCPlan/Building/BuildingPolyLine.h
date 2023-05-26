#pragma once

#include "../BuildingStructsActor.h"

class BuildingPolyLine {
public:
	BuildingPolyLine();
	~BuildingPolyLine();

	static std::tuple<FBuildingBlueprint, FBuildData> BuildAlongPolyLine(
		FBuildingBlueprint blueprint, FBuildData buildData,
		TArray<FVector> vertices, TArray<int> heightFloorsOrder, float unitDiameter, float floorHeight,
		int crossCountUnits = 2, int minCrossCountUnits = -1, int maxCrossCountUnits = -1,
		int numFloors = 1, int minUnits = 1, int maxUnits = -1, int unitCount = -1,
		int maxUnitsRoof = -1, int maxFloors = -1,
		FString unitShape = "hex", int isClosedLoop = 1, float spacing = 0,
		FString crossDirection = "outToIn", bool placeOnSides = false,
		int maxTries = 2, int numTries = 0);
	static std::tuple<FBuildingBlueprint, FBuildData> BuildPositionsCrossSectionColumns(
		FBuildingBlueprint blueprint, FBuildData buildData, int floors,
		FVector posGround, float floorHeight, int crossCountUnits,
		float unitDiameter, FVector mainAxisOneUnit, FVector centerAxisOneUnit,
		int unitCount, float degreesStep = 60, float placeMultipleRangeDegrees = 0,
		bool placeOnSides = false, int maxFloors = -1);
	static std::tuple<FBuildingBlueprint, FBuildData> PlaceAdjacentSpots(int widthCount,
		FBuildingBlueprint blueprint, FBuildData buildData,
		int floors, FVector posGround, float floorHeight, FVector direction, float unitDiameter,
		FVector directionValid, float degreesStep, float placeMultipleRangeDegrees = 0,
		bool firstTime = true, bool placeOnSides = false, int maxFloors = -1);
	static TArray<FVector> FindAdjacentSpots(FVector pos, FVector direction, float unitDiameter,
		FVector directionValid, float degreesStep, float placeMultipleRangeDegrees = 0,
		bool placeOnSides = false, float scaleMagnitude = -1);
	static std::tuple<FBuildingBlueprint, FBuildData> BuildPositionsSingleColumn(
		FBuildingBlueprint blueprint, FBuildData buildData,
		int floors, FVector posCurrentGround, float floorHeight, float unitDiameter,
		TArray<FVector> openEdgePositions, float degreesStep, FString edgeType,
		bool ensureAtLeastOne = true, int maxFloors = -1);
	static bool IsFilledCoord(TArray<FVector2D> filledCoords, FVector2D checkCoord);

	static std::tuple<int, int, float, float, int> EstimateUnitLengthAndWidth(TArray<FVector> vertices,
		TArray<int> heightFloorsOrder,
		int unitCount, float unitDiameter, int minRingWidthUnits = 1, int maxRingWidthUnits = 5,
		float unitLengthsEstimateFactor = 1.1);
	static TArray<int> GetFloorHeightsOneSide(int floorStart, int floorEnd, int numLengths);
};
