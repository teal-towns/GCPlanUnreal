#include "BuildingPolyLine.h"

#include <algorithm>
#include "JsonObjectConverter.h"
#include "math.h"
#include "Math/Vector.h"
#include "Math/Vector2D.h"
#include <random>

#include "../BuildingStructsActor.h"
#include "../Common/DataConvert.h"
#include "../Common/Lodash.h"
#include "../Common/MathPolygon.h"
#include "../Common/MathVector.h"
#include "../Landscape/HeightMap.h"

BuildingPolyLine::BuildingPolyLine() {
}

BuildingPolyLine::~BuildingPolyLine() {
}

// TODO - handle non hex case too.
std::tuple<FBuildingBlueprint, FBuildData> BuildingPolyLine::BuildAlongPolyLine(
	FBuildingBlueprint blueprint, FBuildData buildData,
	TArray<FVector> vertices, TArray<int> heightFloorsOrder, float unitDiameter, float floorHeight,
	int crossCountUnits, int minCrossCountUnits, int maxCrossCountUnits, int numFloors, int minUnits, int maxUnits,
	int unitCount, int maxUnitsRoof, int maxFloors,
	FString unitShape, int isClosedLoop, float spacing,
	FString crossDirection, bool placeOnSides,
	int maxTries, int numTries) {

	int unitLengthsPerHeightSide = -1;
	int unitLengthsEstimate = -1;
	int averageNumFloors = -1;
	bool useHeightFloors = heightFloorsOrder.Num() > 1 ? true : false;
	if (useHeightFloors) {
		auto [ringWidthUnits1, unitLengthsEstimate1, unitLengthsPerHeightSide1, averageNumFloors1, maxFloors1] =
			EstimateUnitLengthAndWidth(vertices, heightFloorsOrder, unitCount, unitDiameter,
			minCrossCountUnits, maxCrossCountUnits);
		crossCountUnits = ringWidthUnits1;
		unitLengthsEstimate = unitLengthsEstimate1;
		averageNumFloors = averageNumFloors1;
		maxFloors = maxFloors1;
		unitLengthsPerHeightSide = unitLengthsPerHeightSide1;
		blueprint.stats.ringWidthUnits = crossCountUnits;
	}

	int unitsCreatedStart = buildData.unitsCreated;
	int unitsCreatedRoofStart = blueprint.stats.roofUnitCount;
	float degreesStep = 360 / 6;
	int vertexCurIndex, vertexNextIndex;
	FVector pathEndCurrent, pathStartCurrent, pathLine, posCurrentGround, posPastGround,
		mainAxis, backAxis, centerAxisOneUnit;
	// Default to a valid starting angle (this is hardcoded and depends on the prefab rotation).
	FVector mainAxisOneUnit = FVector(1,0,0) * unitDiameter;
	FVector mainAxisOneUnitPast, mainAxisOneUnitTemp;
	float pathDistanceRemaining;
	int unitsToPlace;
	bool firstPlacement = true;
	float globalAngle, stepAngle, mainAxisOneUnitLength;
	int heightFloorsIndex = 0;

	float verticesAngle, degreesPerPoint, curDegrees, nextHeightDegrees, curHeightDegrees=0;//smm230623
	int signDirectionHeightDegrees=1, nextHeightDegreesIndex=0, curHeightDegreesIndex=0;//smm230623
	TArray<float> heightFloorsDegrees = {};
	FVector heightStartLine;
	TArray<int> floorHeightPattern;
	int curFloorHeightPatternIndex = 0;
	if (useHeightFloors) {
		// We need to go in the same direction as the vertices shape so check the angle (positive
		// or negative) between the first two vertices.
		verticesAngle = MathVector::SignedAngle((vertices[0] - buildData.posCenterGround),
			(vertices[1] - buildData.posCenterGround), FVector(0,0,1));
		signDirectionHeightDegrees = (verticesAngle > 0) ? 1 : -1;
		degreesPerPoint = 360 / heightFloorsOrder.Num();
		curDegrees = signDirectionHeightDegrees >= 0 ? 0 : 360;
		heightFloorsDegrees = {};
		for (int ii = 0; ii < heightFloorsOrder.Num(); ii++) {
			heightFloorsDegrees.Add(curDegrees);
			curDegrees += signDirectionHeightDegrees * degreesPerPoint;
		}
		// Start at 0 so next is index 1.
		nextHeightDegreesIndex = 1;
		curHeightDegreesIndex = 0;
		nextHeightDegrees = heightFloorsDegrees[nextHeightDegreesIndex];
		heightStartLine = vertices[0] - buildData.posCenterGround;
		floorHeightPattern = GetFloorHeightsOneSide(heightFloorsOrder[curHeightDegreesIndex],
			heightFloorsOrder[nextHeightDegreesIndex], (int)unitLengthsPerHeightSide);
	}

	// Calculate total length to figure out what max iterations should be.
	int iterations = 0;
	float totalLength = 0;
	for (int vv = 0; vv < vertices.Num(); vv++) {
		vertexCurIndex = vv;
		vertexNextIndex = vv + 1;
		if (vertexNextIndex >= vertices.Num()) {
			if (isClosedLoop > 0) {
				vertexNextIndex = 0;
			} else {
				break;
			}
		}
		pathEndCurrent = vertices[vertexNextIndex];
		pathStartCurrent = vertices[vertexCurIndex];
		pathLine = pathEndCurrent - pathStartCurrent;
		totalLength += pathLine.Size();
	}
	int maxIterations = (int)ceil(totalLength / unitDiameter) + 10;
	int valid = 1;
	posCurrentGround = vertices[0];
	buildData.posCurrentGround = vertices[0];
	posPastGround = vertices[0];
	FVector posCenterGround = buildData.posCenterGround;
	bool done = false;

	for (int vv = 0; vv < vertices.Num(); vv++) {
		vertexCurIndex = vv;
		vertexNextIndex = vv + 1;
		if (vertexNextIndex >= vertices.Num()) {
			if (isClosedLoop < 1) {
				break;
			}
			vertexNextIndex = 0;
		}
		pathEndCurrent = vertices[vertexNextIndex];
		pathStartCurrent = vertices[vertexCurIndex];
		pathLine = pathEndCurrent - pathStartCurrent;

		iterations = 0;
		while (true) {
			posCurrentGround = buildData.posCurrentGround;
			pathDistanceRemaining = (pathEndCurrent - posCurrentGround).Size();
			mainAxisOneUnitPast = mainAxisOneUnit;

			if (firstPlacement || iterations == 0) {
				mainAxis = pathEndCurrent - posCurrentGround;
				globalAngle = MathVector::SignedAngle(mainAxisOneUnitPast, mainAxis, FVector(0,0,1));
				stepAngle = MathPolygon::StepAngle(globalAngle, degreesStep);
				mainAxisOneUnit = mainAxisOneUnit.RotateAngleAxis(stepAngle, FVector(0,0,1)).GetClampedToMaxSize(unitDiameter);
			}
			if (!firstPlacement && iterations != 0) {
				// Find the desired future point by projecting current path onto vertex edge
				// (desired) path, then moving up one unit.
				FVector curDirection = posCurrentGround - pathStartCurrent;
				FVector projection = curDirection.ProjectOnTo(pathLine);
				FVector projectionNextUnit = projection + projection.GetUnsafeNormal() * unitDiameter;
				FVector nextPointOnPath = pathStartCurrent + projectionNextUnit;
				mainAxis = nextPointOnPath - posCurrentGround;
				globalAngle = MathVector::SignedAngle(mainAxisOneUnitPast, mainAxis, FVector(0,0,1));
				stepAngle = MathPolygon::StepAngle(globalAngle, degreesStep);

				mainAxisOneUnitTemp = mainAxisOneUnitPast.RotateAngleAxis(stepAngle, FVector(0,0,1)).GetClampedToMaxSize(unitDiameter);
				mainAxisOneUnitLength = mainAxisOneUnitTemp.Size();
				if (mainAxisOneUnitLength < unitDiameter) {
					mainAxisOneUnitTemp *= unitDiameter / mainAxisOneUnitLength;
				}
				if (pathDistanceRemaining < unitDiameter * 0.5f) {
					break;
				}
				mainAxisOneUnit = mainAxisOneUnitTemp;
			}

			if (crossDirection == "inToOut") {
				centerAxisOneUnit = (posCurrentGround - posCenterGround).GetClampedToMaxSize(unitDiameter);
			} else {
				centerAxisOneUnit = (posCenterGround - posCurrentGround).GetClampedToMaxSize(unitDiameter);
			}

			if (useHeightFloors) {
				curHeightDegrees = MathVector::SignedAngle(heightStartLine, (posCurrentGround - posCenterGround), FVector(0,0,1));
				// Do NOT want signed angle and want to get above 180 degrees.
				if (curHeightDegrees < 0) {
					curHeightDegrees = 360 + curHeightDegrees;
				}
				if (signDirectionHeightDegrees < 0 && curHeightDegrees <= 0) {
					curHeightDegrees = 360 + curHeightDegrees;
				}
				numFloors = floorHeightPattern[curFloorHeightPatternIndex];
			}

			unitsToPlace = numFloors * crossCountUnits;
			auto [blueprint1, buildData1] = BuildPositionsCrossSectionColumns(blueprint, buildData, numFloors,
				buildData.posCurrentGround, floorHeight, crossCountUnits,
				unitDiameter, mainAxisOneUnit, centerAxisOneUnit, unitsToPlace,
				degreesStep, 0, placeOnSides, maxFloors);
			blueprint = blueprint1;
			buildData = buildData1;

			firstPlacement = false;
			posPastGround = posCurrentGround;
			posCurrentGround += mainAxisOneUnit;
			buildData.posCurrentGround = posCurrentGround;

			if (useHeightFloors) {
				if (curFloorHeightPatternIndex < floorHeightPattern.Num() - 1) {
						curFloorHeightPatternIndex += 1;
				}
				// If positive direction (e.g. 0, 90, 180, 270), then go to 2nd index when > 90.
				// If negative direction (e.g. 0, -90, -180, -270), go to 2nd index when < -90
				// Stop once get to last one (get through one cycle).
				if (curHeightDegreesIndex < heightFloorsOrder.Num() - 1 &&
					((signDirectionHeightDegrees < 0 && curHeightDegrees < heightFloorsDegrees[nextHeightDegreesIndex]) ||
					(signDirectionHeightDegrees >= 0 && curHeightDegrees > heightFloorsDegrees[nextHeightDegreesIndex]))) {
					nextHeightDegreesIndex += 1;
					curHeightDegreesIndex += 1;
					if (nextHeightDegreesIndex >= heightFloorsDegrees.Num()) {
						nextHeightDegreesIndex = 0;
					}
					if (curHeightDegreesIndex >= heightFloorsDegrees.Num()) {
						curHeightDegreesIndex = 0;
					}
					floorHeightPattern = GetFloorHeightsOneSide(heightFloorsOrder[curHeightDegreesIndex],
						heightFloorsOrder[nextHeightDegreesIndex], (int)unitLengthsPerHeightSide);
					curFloorHeightPatternIndex = 0;
				}
			}

			if (maxUnits > 0 && (buildData.unitsCreated - unitsCreatedStart) >= maxUnits) {
				done = true;
				break;
			}
			if (maxUnitsRoof > 0 && (blueprint.stats.roofUnitCount - unitsCreatedRoofStart) >= maxUnitsRoof) {
				done = true;
				break;
			}
			if (pathDistanceRemaining < unitDiameter * 0.67f) {
				break;
			}

			iterations += 1;
			if (iterations > maxIterations) {
				numTries += 1;
				UE_LOG(LogTemp, Warning, TEXT("BLPolyLine.BuildAlongPolyLine failsafe iterations break"));
				valid = 0;
				if (numTries < maxTries) {
					return BuildAlongPolyLine(blueprint, buildData, vertices, heightFloorsOrder,
						unitDiameter, floorHeight,
						crossCountUnits, minCrossCountUnits, maxCrossCountUnits,
						numFloors, minUnits, maxUnits, unitCount, maxUnitsRoof, maxFloors,
						unitShape, isClosedLoop, spacing, crossDirection, placeOnSides,
						maxTries, numTries);
				} else {
					UE_LOG(LogTemp, Warning, TEXT("BLPolyLine.BuildAlongPolyLine error, skipping"));
				}
				break;
			}
		}
		if (valid <= 0 || done) {
			break;
		}
	}

	blueprint.valid = valid;

	return {blueprint, buildData};
}

// Find positions for a line of units perpendicular to the shape line,
// moving along the cross axis.
// E.g. for cross units of 3, this would position a unit on the given position and then
// 2 units next to it.
std::tuple<FBuildingBlueprint, FBuildData> BuildingPolyLine::BuildPositionsCrossSectionColumns(
	FBuildingBlueprint blueprint, FBuildData buildData, int floors,
	FVector posGround, float floorHeight, int crossCountUnits,
	float unitDiameter, FVector mainAxisOneUnit, FVector centerAxisOneUnit,
	int unitCount, float degreesStep, float placeMultipleRangeDegrees,
	bool placeOnSides, int maxFloors) {
	int unitsCreatedStart = buildData.unitsCreated;
	int roofsCreatedStart = blueprint.stats.roofUnitCount;
	auto [blueprint1, buildData1] = PlaceAdjacentSpots(crossCountUnits, blueprint, buildData,
		floors, posGround, floorHeight, centerAxisOneUnit, unitDiameter, mainAxisOneUnit,
		degreesStep, placeMultipleRangeDegrees, placeOnSides, maxFloors);
	blueprint = blueprint1;
	buildData = buildData1;
	return {blueprint, buildData};
}

// Recursive function to place adjacent units in a line widthCount long.
std::tuple<FBuildingBlueprint, FBuildData> BuildingPolyLine::PlaceAdjacentSpots(int widthCount,
	FBuildingBlueprint blueprint, FBuildData buildData,
	int floors, FVector posGround, float floorHeight, FVector direction, float unitDiameter,
	FVector directionValid, float degreesStep, float placeMultipleRangeDegrees,
	bool placeOnSides, int maxFloors, bool firstTime) {
	float placeMultipleRangeDegreesNext = 0;
	// Place this column.
	FVector2D coord = FVector2D(posGround.X, posGround.Y);
	bool alreadyFilled = IsFilledCoord(buildData.filledCoords, coord);
	int roofsCountStart = blueprint.stats.roofUnitCount;
	if (!alreadyFilled) {
		TArray<FVector> openEdgePositions = {};
		TArray<FVector> directionsTemp;
		FString edgeType = "";
		// Find open edges.
		if (firstTime) {
			// Outer edges.
			FVector directionBackward = direction.RotateAngleAxis(180, FVector(0,0,1));
			directionsTemp = FindAdjacentSpots(posGround, directionBackward, unitDiameter,
				directionValid, degreesStep, placeMultipleRangeDegrees, true,
				unitDiameter * 0.5f);
			openEdgePositions += directionsTemp;
			edgeType = "outer";
		}
		if (widthCount == 1) {
			// Inner edges.
			directionsTemp = FindAdjacentSpots(posGround, direction, unitDiameter,
				directionValid, degreesStep, placeMultipleRangeDegrees, true,
				unitDiameter * 0.5f);
			openEdgePositions += directionsTemp;
			edgeType = "inner";
		}
		auto [blueprint1, buildData1] = BuildPositionsSingleColumn(blueprint, buildData, floors,
			posGround, floorHeight, unitDiameter, openEdgePositions, degreesStep, edgeType,
			true, maxFloors);
		blueprint = blueprint1;
		buildData = buildData1;
		buildData.filledCoords.Add(coord);
		// if (firstTime) {
		//	 buildData.unitLengthsCreatedOriginRing += 1;
		// }
	}
	if (!alreadyFilled && widthCount > 1) {
	// If have more to place, find adjacent then recursively call to place them too.
		FVector posGroundNew, directionNew, directionValidNew;
		TArray<FVector> adjacentSpots = FindAdjacentSpots(posGround, direction, unitDiameter,
			directionValid, degreesStep, placeMultipleRangeDegrees, placeOnSides);
		for (int ii = 0; ii < adjacentSpots.Num(); ii++) {
			posGroundNew = adjacentSpots[ii];
			directionNew = direction;
			directionValidNew = posGroundNew - posGround;
			auto [blueprint1, buildData1] = PlaceAdjacentSpots((widthCount - 1), blueprint, buildData,
				floors, posGroundNew, floorHeight, directionNew, unitDiameter, directionValidNew,
				degreesStep, placeMultipleRangeDegreesNext, placeOnSides, maxFloors, false);
			blueprint = blueprint1;
			buildData = buildData1;
		}
	}
	return {blueprint, buildData};
}

TArray<FVector> BuildingPolyLine::FindAdjacentSpots(FVector pos, FVector direction, float unitDiameter,
	FVector directionValid, float degreesStep, float placeMultipleRangeDegrees,
	bool placeOnSides, float scaleMagnitude) {

	float angleDiff = MathVector::SignedAngle(directionValid, direction, FVector(0,0,1));
	float stepAngle = MathPolygon::StepAngle(angleDiff, degreesStep);
	FVector directionToUse = directionValid.RotateAngleAxis(stepAngle, FVector(0,0,1)).GetClampedToMaxSize(unitDiameter);
	float angleAdjust = MathVector::SignedAngle(direction, directionToUse, FVector(0,0,1));
	TArray<FVector> spots = {};
	if (scaleMagnitude > 0) {
		directionToUse = directionToUse.GetUnsafeNormal() * scaleMagnitude;
	}
	spots.Add(pos + directionToUse);
	FVector directionMultiple;
	if (placeOnSides) {
		directionMultiple = directionValid.RotateAngleAxis(stepAngle + degreesStep, FVector(0,0,1));
		if (scaleMagnitude > 0) {
			directionMultiple = directionMultiple.GetUnsafeNormal() * scaleMagnitude;
		}
		spots.Add(pos + directionMultiple);
	}
	if (placeOnSides) {
		directionMultiple = directionValid.RotateAngleAxis(stepAngle - degreesStep, FVector(0,0,1));
		if (scaleMagnitude > 0) {
			directionMultiple = directionMultiple.GetUnsafeNormal() * scaleMagnitude;
		}
		spots.Add(pos + directionMultiple);
	}
	return spots;
}

std::tuple<FBuildingBlueprint, FBuildData> BuildingPolyLine::BuildPositionsSingleColumn(
	FBuildingBlueprint blueprint, FBuildData buildData,
	int floors, FVector posCurrentGround, float floorHeight, float unitDiameter,
	TArray<FVector> openEdgePositions, float degreesStep, FString edgeType,
	bool ensureAtLeastOne, int maxFloors) {
	int floorsToUse = floors;
	FString name, uniqueName, prefabName;
	FVector pos;
	FVector scale = FVector(1.15f, 1, 1.15f);
	FRotator rot = FRotator(0, 0, 0);
	// LandPrefabClass prefab;
	HeightMap* heightMap = HeightMap::GetInstance();
	float terrainZ = heightMap->GetTerrainHeightAtPoint(posCurrentGround);
	// https://forums.unrealengine.com/t/how-to-get-terrain-height-at-x-and-y-coordinates-in-c/447803
	// float terrainZ = HeightMap::GetTerrainHeightAtPoint(posCurrentGround);
	// float terrainZ = _cmtGetTerrain.GetTerrainYAtPoint(posCurrentGround);
	// float terrainZ = 200;
	float posZ = posCurrentGround.Z;
	TArray<FString> emptyTArray = {};

	TArray<FString> prefabNames;
	float roofRadius = unitDiameter * 0.5f;
	TArray<FVector> roofVertices = {};
	FVector scaleTemp, posTemp;
	int unitsCreatedStart = buildData.unitsCreated;

	// Check for extra lower floors first.
	while (posZ > terrainZ) {
		posZ -= floorHeight;
		floorsToUse += 1;
	}
	if (ensureAtLeastOne && floorsToUse <= 1 && posZ < terrainZ) {
		float posZTemp = posZ;
		while (posZTemp < terrainZ) {
			floorsToUse += 1;
			posZTemp += floorHeight;
		}
	}

	bool firstFloorPlaced = false;
	int floorNum = 1;
	int floorNumAboveGround = 1;
	for (int ii = 0; ii < floorsToUse; ii++) {
		bool aboveGround = true;
		bool isRoof = false;
		int floorNumToUse = floorNum;
		// See if (partially) below ground.
		if (posZ >= terrainZ - floorHeight) {
			if (posZ >= terrainZ) {
				if (ii == (floorsToUse - 1)) {
					prefabName = "HexModuleNatureRoofLod";
					isRoof = true;
				} else {
					prefabName = "HexModuleLod";
				}
				// prefabName = "HexModulePerf1";

				buildData.unitsCreated += 1;
				blueprint.stats.unitCount += 1;
				floorNum += 1;
				if (posZ >= terrainZ + floorHeight) {
					floorNumAboveGround += 1;
				}
			} else {
				prefabName = "HexModuleRaiserLodWrap";
				aboveGround = false;
				floorNumToUse = 0;
			}

			name = prefabName;
			uniqueName = Lodash::GetInstanceId(name);
			pos = FVector(posCurrentGround.X, posCurrentGround.Y, posZ);

			FBuildingBlueprintJsonData jsonData = FBuildingBlueprintJsonData(
				DataConvert::VectorsToStrings(openEdgePositions), floorNumToUse, edgeType);
			FString JsonString;
			FJsonObjectConverter::UStructToJsonObjectString(jsonData, JsonString, 0, 0);

			blueprint.land.game_objects.Add(uniqueName, FLandGameObject(name,
				pos, rot, scale, emptyTArray, JsonString));

			firstFloorPlaced = true;

			if (maxFloors > 0 && floorNumAboveGround > maxFloors) {
				break;
			}
		// } else {
		//	 Debug.LogWarning("NOT above ground, skipping " + ii + " " + floorsToUse);
		}

		posZ += floorHeight;
	}
	if (buildData.unitsCreated > unitsCreatedStart) {
		blueprint.stats.roofUnitCount += 1;
	}

	return {blueprint, buildData};
}

bool BuildingPolyLine::IsFilledCoord(TArray<FVector2D> filledCoords, FVector2D checkCoord) {
	FVector2D coord;
	for (int ii = 0; ii < filledCoords.Num(); ii++) {
		coord = filledCoords[ii];
		if (checkCoord.X > floor(coord.X) && checkCoord.X < ceil(coord.X) &&
			checkCoord.Y > floor(coord.Y) && checkCoord.Y < ceil(coord.Y)) {
			return true;
		}
	}
	return false;
}

TArray<int> BuildingPolyLine::GetFloorHeightsOneSide(int floorStart, int floorEnd, int numLengths) {
	TArray<int> floorPattern = {};
	int floorLow, floorHigh;
	float floorIncrementVal;
	if (floorStart < floorEnd) {
		floorLow = floorStart;
		floorHigh = floorEnd;
		floorIncrementVal = 1;
	} else {
		floorLow = floorEnd;
		floorHigh = floorStart;
		floorIncrementVal = -1;
	}
	floorIncrementVal *= (float)(floorHigh - floorLow + 1) / (float)numLengths;
	float curFloor = (float)floorStart;
	for (int ii = 0; ii < numLengths; ii++) {
		floorPattern.Add((int)round(curFloor));
		curFloor += floorIncrementVal;
		if (curFloor > floorHigh) {
			curFloor = floorHigh;
		}
		if (curFloor < floorLow) {
			curFloor = floorLow;
		}
	}
	return floorPattern;
}

std::tuple<int, int, float, float, int> BuildingPolyLine::EstimateUnitLengthAndWidth(TArray<FVector> vertices,
	TArray<int> heightFloorsOrder,
	int unitCount, float unitDiameter, int minRingWidthUnits, int maxRingWidthUnits,
	float unitLengthsEstimateFactor) {
	// Get total distance between vertices to estimate total unit lengths so we know how
	// divide up floor highs and lows.
	float distance = 0;
	for (int vv = 0; vv < vertices.Num(); vv++) {
		if (vv == vertices.Num() - 1) {
			distance += (vertices[0] - vertices[vv]).Size();
		} else {
			distance += (vertices[(vv + 1)] - vertices[vv]).Size();
		}
	}

	int unitLengthsEstimate = (int)floor(distance / unitDiameter * unitLengthsEstimateFactor);
	float unitLengthsPerHeightSide = unitLengthsEstimate / heightFloorsOrder.Num();

	float sum = 0;
	int maxFloors = -1;
	for (int ii = 0; ii < heightFloorsOrder.Num(); ii++) {
		sum += heightFloorsOrder[ii];
		if (maxFloors == -1 || heightFloorsOrder[ii] > maxFloors) {
			maxFloors = heightFloorsOrder[ii];
		}
	}
	float averageNumFloors = sum / heightFloorsOrder.Num();
	int unitCountOriginRingEstimate = (int)(unitLengthsEstimate * averageNumFloors);
	int ringWidthUnits = (int)round(unitCount / unitCountOriginRingEstimate);
	ringWidthUnits = std::max(minRingWidthUnits, std::min(maxRingWidthUnits, ringWidthUnits));

	return {ringWidthUnits, unitLengthsEstimate, unitLengthsPerHeightSide, averageNumFloors, maxFloors};
}
