#include "BuildingFlowerHomes.h"

#include "JsonObjectConverter.h"

#include "../BuildingStructsActor.h"
#include "../Common/DataConvert.h"
#include "../Common/Lodash.h"
#include "../Common/MathPolygon.h"
#include "../Common/MathVector.h"
#include "../Landscape/HeightMap.h"
#include "BuildingPolyLine.h"

BuildingFlowerHomes::BuildingFlowerHomes() {
}

BuildingFlowerHomes::~BuildingFlowerHomes() {
}

std::tuple<FBuildingBlueprint, TMap<FString, FRoadPath>> BuildingFlowerHomes::Create(
	TArray<FVector> pathVertices, float verticesBuffer, float centerAreaRatio,
	float homeLandSizeSqM, FString homeLocation,
	int maxFloors, FString uName, float unitDiameter, float floorHeight,
	int maxUnitsRoof, int crossCountUnits, float minHomeLandSizeSqM) {
	FBuildingBlueprint blueprint = FBuildingBlueprint();
	if (uName.Len() == 0) {
		uName = Lodash::GetInstanceId();
	}
	blueprint.land.land_id = "FlowerHomes_" + uName;

	FVector posCenterGround = MathPolygon::GetPolygonCenter(pathVertices);
	blueprint.posCenterGround = posCenterGround;
	// Find point outside the vertices for opposite direction.
	// FVector posOppositeCenter = pathVertices[0] + (posVertices[0] - posCenterGround);
	FVector buildTowardPoint;

	TMap<FString, FRoadPath> homePlotPaths = {};
	// Un buffer vertices for paths to extend all the way to roads.
	TArray<FVector> spacesVertices = MathPolygon::BufferVertices(pathVertices, posCenterGround, -1 * verticesBuffer);

	TArray<FVector2D> vertices2D = MathPolygon::PointsTo2D(pathVertices);
	float area = MathPolygon::PolygonArea(vertices2D);
	float homesArea = area * (1 - centerAreaRatio);
	// int homesCount = (int)round(homesArea / homeLandSizeSqM);
	// int degreesPerHome = (int)floor(360 / homesCount);
	auto [radiusAverage, radiusMin] = MathPolygon::GetAverageRadius(pathVertices, posCenterGround);
	float radiusInner = MathPolygon::GetCircleRadiusByArea(radiusAverage, centerAreaRatio);

	blueprint.stats.averageRadiusOuter = radiusAverage;
	blueprint.stats.averageRadiusInner = radiusInner;
	FBuildData buildData = FBuildData(posCenterGround, posCenterGround);

	float verticesCount = pathVertices.Num();
	TArray<FVector> verticesInnerCircle = {};
	// Draw a line from each vertex toward the center to form both the center and each home land.
	FVector pathFull, path, vertex, pathInner, vertexInner, vertexNext, edgeCenter, vertexInnerCirclePrevious, currentTemp, nextTemp;
	FString uNameTemp;
	TArray<FVector> verticesLine;
	for (int ii = 0; ii < verticesCount; ii++) {
		vertex = pathVertices[ii];
		int indexNext = (ii < verticesCount - 1) ? ii + 1 : 0;
		vertexNext = pathVertices[indexNext];
		pathFull = vertex - posCenterGround;
		vertexInner = posCenterGround + pathFull.GetClampedToMaxSize(radiusInner);
		verticesInnerCircle.Add(vertexInner);
		path = vertex - vertexInner;

		// Add inner circle path.
		if (ii >= 1) {
			currentTemp = verticesInnerCircle[(ii - 1)];
			nextTemp = verticesInnerCircle[ii];
			edgeCenter = currentTemp + (nextTemp - currentTemp) * 0.5;
			uNameTemp = "BuildingPath_" + Lodash::ToFixed(edgeCenter.X, 1) + "_" + Lodash::ToFixed(edgeCenter.Y, 1);
			homePlotPaths.Add(uNameTemp, FRoadPath(uNameTemp, uNameTemp, { currentTemp, nextTemp }, 3, "walkPath"));
		}
		// Since we start at 2nd one, on that last one, write the last one too.
		if (ii == (verticesCount - 1)) {
			currentTemp = verticesInnerCircle[ii];
			nextTemp = verticesInnerCircle[0];
			edgeCenter = currentTemp + (nextTemp - currentTemp) * 0.5;
			uNameTemp = "BuildingPath_" + Lodash::ToFixed(edgeCenter.X, 1) + "_" + Lodash::ToFixed(edgeCenter.Y, 1);
			homePlotPaths.Add(uNameTemp, FRoadPath(uNameTemp, uNameTemp, { currentTemp, nextTemp }, 3, "walkPath"));
		}

		// Check that plot is big enough (on plot edges will be squished / small).
		pathFull = vertexNext - posCenterGround;
		FVector vertexInnerNext = posCenterGround + pathFull.GetClampedToMaxSize(radiusInner);
		TArray<FVector2D> homeVertices = { FVector2D(vertexInner.X, vertexInner.Y),
			FVector2D(vertexInnerNext.X, vertexInnerNext.Y), FVector2D(vertexNext.X, vertexNext.Y),
			FVector2D(vertex.X, vertex.Y) };
		float homeArea = MathPolygon::PolygonArea(homeVertices);
		if (homeArea < minHomeLandSizeSqM) {
			UE_LOG(LogTemp, Warning, TEXT("BuildingFlowerHomes.Create home area too small, skipping %f %f"), homeArea, minHomeLandSizeSqM);
		} else {
			// Use non buffered vertex point for path.
			currentTemp = spacesVertices[ii];
			nextTemp = vertexInner;
			edgeCenter = currentTemp + (nextTemp - currentTemp) * 0.5;
			uNameTemp = "BuildingPath_" + Lodash::ToFixed(edgeCenter.X, 1) + "_" + Lodash::ToFixed(edgeCenter.Y, 1);
			homePlotPaths.Add(uNameTemp, FRoadPath(uNameTemp, uNameTemp, { currentTemp, nextTemp }, 3, "walkPath"));

			TArray<int> heightFloorsOrder = {};
			int minCrossCountUnits = -1;
			int maxCrossCountUnits = -1;
			int unitCount = -1;
			if (homeLocation == "outer") {
				verticesLine = {vertex, vertexNext};
				// auto [blueprint1, buildData1] = BuildingPolyLine::BuildAlongPolyLine(blueprint,
				// 	buildData, verticesLine, heightFloorsOrder, unitDiameter, floorHeight,
				// 	crossCountUnits, minCrossCountUnits, maxCrossCountUnits,
				// 	1, 1, -1, unitCount, maxUnitsRoof, maxFloors, "hex", 0);
				// blueprint = blueprint1;
				// buildData = buildData1;
				blueprint = BuildHexHome(blueprint, verticesLine, posCenterGround, maxUnitsRoof,
					maxFloors, unitDiameter, floorHeight);
			} else {
				// Need at least 2 points on inner circle to build, so wait for 2nd one and build previous.
				if (ii >= 1) {
					vertexInnerCirclePrevious = verticesInnerCircle[(ii - 1)];
					verticesLine = {vertexInnerCirclePrevious, verticesInnerCircle[ii]};
					// auto [blueprint1, buildData1] = BuildingPolyLine::BuildAlongPolyLine(blueprint,
					// 	buildData, verticesLine, heightFloorsOrder, unitDiameter, floorHeight,
					// 	crossCountUnits, minCrossCountUnits, maxCrossCountUnits,
					// 	1, 1, -1, unitCount, (maxUnitsRoof - 1), maxFloors, "hex", 0, 0, "inToOut");
					// blueprint = blueprint1;
					// buildData = buildData1;
					// If inner, want to build toward the outside, so pick a vertex to build toward.
					buildTowardPoint = verticesLine[0] + (verticesLine[1] - verticesLine[0]) * 0.5;
					blueprint = BuildHexHome(blueprint, verticesLine, buildTowardPoint, maxUnitsRoof,
						maxFloors, unitDiameter, floorHeight);
				}
				// Since we start at 2nd one, on that last one, write the last one too.
				if (ii == (verticesCount - 1)) {
					verticesLine = { verticesInnerCircle[ii], verticesInnerCircle[0] };
					// auto [blueprint1, buildData1] = BuildingPolyLine::BuildAlongPolyLine(blueprint,
					// 	buildData, verticesLine, heightFloorsOrder, unitDiameter, floorHeight,
					// 	crossCountUnits, minCrossCountUnits, maxCrossCountUnits,
					// 	1, 1, -1, unitCount, (maxUnitsRoof - 1), maxFloors, "hex", 0, 0, "inToOut");
					// blueprint = blueprint1;
					// buildData = buildData1;
					buildTowardPoint = verticesLine[0] + (verticesLine[1] - verticesLine[0]) * 0.5;
					blueprint = BuildHexHome(blueprint, verticesLine, buildTowardPoint, maxUnitsRoof,
						maxFloors, unitDiameter, floorHeight);
				}
			}
		}
	}

	return {blueprint, homePlotPaths};
}

FBuildingBlueprint BuildingFlowerHomes::BuildHexHome(FBuildingBlueprint blueprint,
	TArray<FVector> pathVertices, FVector innerPoint,
	int numRoofUnits, int numFloors, float unitDiameter, float floorHeight) {
	float degreesStep = 360 / 6;
	HeightMap* heightMap = HeightMap::GetInstance();
	FVector posCurrentGround = pathVertices[0] + (pathVertices[1] - pathVertices[0]) * 0.5;
	posCurrentGround.Z = heightMap->GetTerrainHeightAtPoint(posCurrentGround);

	FVector mainAxis, mainAxisOneUnitPast;
	// Default to a valid starting angle (this is hardcoded and depends on the prefab rotation).
	FVector mainAxisOneUnit = FVector(1,0,0) * unitDiameter;
	float globalAngle, stepAngle;

	TArray<FVector> unitPositions = {};
	mainAxis = pathVertices[1] - pathVertices[0];

	// Place first in the middle.
	blueprint = BuildColumn(blueprint, posCurrentGround, numFloors, floorHeight);
	unitPositions.Add(posCurrentGround);

	// Place 2nd next to it (along line).
	mainAxisOneUnitPast = mainAxisOneUnit;
	globalAngle = MathVector::SignedAngle(mainAxisOneUnitPast, mainAxis, FVector(0,0,1));
	stepAngle = MathPolygon::StepAngle(globalAngle, degreesStep);
	mainAxisOneUnit = mainAxisOneUnit.RotateAngleAxis(stepAngle, FVector(0,0,1)).GetClampedToMaxSize(unitDiameter);
	posCurrentGround += mainAxisOneUnit;
	unitPositions.Add(posCurrentGround);
	blueprint = BuildColumn(blueprint, posCurrentGround, numFloors, floorHeight);

	// Place 3rd in front in between the first two.
	mainAxisOneUnit = unitPositions[1] - unitPositions[0];
	FVector centerAxisOneUnit = (innerPoint - unitPositions[0]).GetClampedToMaxSize(unitDiameter);
	// See if angle to center is positive or negative.
	globalAngle = MathVector::SignedAngle(mainAxisOneUnit, centerAxisOneUnit, FVector(0,0,1));
	int sign = globalAngle < 0 ? -1 : 1;
	// globalAngle = MathVector::SignedAngle(centerAxisOneUnit, mainAxis, FVector(0,0,1));
	// stepAngle = MathPolygon::StepAngle(globalAngle, degreesStep);
	mainAxisOneUnit = mainAxisOneUnit.RotateAngleAxis(sign * degreesStep, FVector(0,0,1)).GetClampedToMaxSize(unitDiameter);
	posCurrentGround = unitPositions[0] + mainAxisOneUnit;
	unitPositions.Add(posCurrentGround);
	blueprint = BuildColumn(blueprint, posCurrentGround, numFloors, floorHeight);

	// Place 4th in front next to the 3rd (just move backwards main axis one unit from 3rd).
	if (numRoofUnits >= 4) {
		mainAxisOneUnit = unitPositions[0] - unitPositions[1];
		posCurrentGround = unitPositions[2] + mainAxisOneUnit;
		unitPositions.Add(posCurrentGround);
		blueprint = BuildColumn(blueprint, posCurrentGround, numFloors, floorHeight);
	}

	// Place 5th in back next to the first one (just move backwards main axis from 1st).
	if (numRoofUnits >= 5) {
		mainAxisOneUnit = unitPositions[0] - unitPositions[1];
		posCurrentGround = unitPositions[0] + mainAxisOneUnit;
		unitPositions.Add(posCurrentGround);
		blueprint = BuildColumn(blueprint, posCurrentGround, numFloors, floorHeight);
	}

	return blueprint;
}

FBuildingBlueprint BuildingFlowerHomes::BuildColumn(FBuildingBlueprint blueprint,
	FVector posCurrentGround, int numFloors, float floorHeight) {
	FString name, uniqueName, prefabName;
	FVector pos;
	FVector scale = FVector(1.15f, 1, 1.15f);
	FRotator rot = FRotator(0, 0, 0);
	prefabName = "HexModuleLod";
	TArray<FVector> openEdgePositions = {};
	FString edgeType = "";
	TArray<FString> emptyTArray = {};

	HeightMap* heightMap = HeightMap::GetInstance();
	posCurrentGround.Z = heightMap->GetTerrainHeightAtPoint(posCurrentGround);

	float posZ = posCurrentGround.Z;
	for (int ii = 0; ii < numFloors; ii++) {
		int floorNum = ii + 1;
		name = prefabName;
		uniqueName = Lodash::GetInstanceId(name);
		pos = FVector(posCurrentGround.X, posCurrentGround.Y, posZ);
		FBuildingBlueprintJsonData jsonData = FBuildingBlueprintJsonData(
			DataConvert::VectorsToDicts(openEdgePositions), floorNum, edgeType);
		FString JsonString;
		FJsonObjectConverter::UStructToJsonObjectString(jsonData, JsonString, 0, 0);
		blueprint.land.game_objects.Add(uniqueName, FLandGameObject(name,
			pos, rot, scale, emptyTArray, JsonString));

		posZ += floorHeight;
	}
	return blueprint;
}
