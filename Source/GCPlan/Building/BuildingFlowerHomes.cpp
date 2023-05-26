#include "BuildingFlowerHomes.h"

#include "../BuildingStructsActor.h"
#include "../Common/Lodash.h"
#include "../Common/MathPolygon.h"
#include "BuildingPolyLine.h"

BuildingFlowerHomes::BuildingFlowerHomes() {
}

BuildingFlowerHomes::~BuildingFlowerHomes() {
}

std::tuple<FBuildingBlueprint, TArray<FVector>> BuildingFlowerHomes::Create(TArray<FVector> pathVertices, float centerAreaRatio,
	float homeLandSizeSqM, FString homeLocation, int hexesPerHome,
	int floorsPerHome, int maxFloors, FString uName, float unitDiameter, float floorHeight,
	int maxUnitsRoof, int crossCountUnits) {
	FBuildingBlueprint blueprint = FBuildingBlueprint();
	if (uName.Len() == 0) {
		uName = Lodash::GetInstanceId();
	}
	blueprint.land.land_id = "FlowerHomes_" + uName;

	FVector posCenterGround = MathPolygon::GetPolygonCenter(pathVertices);
	blueprint.posCenterGround = posCenterGround;

	TArray<FVector> homePlotPaths = {};

	TArray<FVector2D> vertices2D = MathPolygon::PointsTo2D(pathVertices);
	float area = MathPolygon::PolygonArea(vertices2D);
	float homesArea = area * (1 - centerAreaRatio);
	int homesCount = (int)round(homesArea / homeLandSizeSqM);
	int degreesPerHome = (int)floor(360 / homesCount);
	auto [radiusAverage, radiusMin] = MathPolygon::GetAverageRadius(pathVertices, posCenterGround);
	float radiusInner = MathPolygon::GetCircleRadiusByArea(radiusAverage, centerAreaRatio);

	blueprint.stats.averageRadiusOuter = radiusAverage;
	blueprint.stats.averageRadiusInner = radiusInner;
	FBuildData buildData = FBuildData(posCenterGround, posCenterGround);

	float verticesCount = pathVertices.Num();
	TArray<FVector> verticesInnerCircle = {};
	// Draw a line from each vertex toward the center to form both the center and each home land.
	FVector pathFull, path, vertex, pathInner, vertexInner, vertexNext;
	for (int ii = 0; ii < verticesCount; ii++) {
		vertex = pathVertices[ii];
		int indexNext = (ii < verticesCount - 1) ? ii + 1 : 0;
		vertexNext = pathVertices[indexNext];
		pathFull = vertex - posCenterGround;
		vertexInner = posCenterGround + pathFull.GetClampedToMaxSize(radiusInner);
		verticesInnerCircle.Add(vertexInner);
		path = vertex - vertexInner;
		homePlotPaths.Add(path);

		TArray<int> heightFloorsOrder = {};
		int minCrossCountUnits = -1;
		int maxCrossCountUnits = -1;
		int unitCount = -1;
		if (homeLocation == "outer") {
			TArray<FVector> verticesLine = {vertex, vertexNext};
			auto [blueprint1, buildData1] = BuildingPolyLine::BuildAlongPolyLine(blueprint,
				buildData, verticesLine, heightFloorsOrder, unitDiameter, floorHeight,
				crossCountUnits, minCrossCountUnits, maxCrossCountUnits,
				1, 1, -1, unitCount, maxUnitsRoof, maxFloors, "hex", 0);
			blueprint = blueprint1;
			buildData = buildData1;
		} else {
			// Need at least 2 points on inner circle to build, so wait for 2nd one and build previous.
			if (ii >= 1) {
				FVector vertexInnerCirclePrevious = verticesInnerCircle[(ii - 1)];
				TArray<FVector> verticesLine = {vertexInnerCirclePrevious, verticesInnerCircle[ii]};
				auto [blueprint1, buildData1] = BuildingPolyLine::BuildAlongPolyLine(blueprint,
					buildData, verticesLine, heightFloorsOrder, unitDiameter, floorHeight,
					crossCountUnits, minCrossCountUnits, maxCrossCountUnits,
					1, 1, -1, unitCount, (maxUnitsRoof - 1), maxFloors, "hex", 0, 0, "inToOut");
				blueprint = blueprint1;
				buildData = buildData1;
			}
			// Since we start at 2nd one, on that last one, write the last one too.
			if (ii == (verticesCount - 1)) {
				FVector vertexInnerCirclePrevious = verticesInnerCircle[(ii - 1)];
				TArray<FVector> verticesLine = { verticesInnerCircle[ii], verticesInnerCircle[0] };
				auto [blueprint1, buildData1] = BuildingPolyLine::BuildAlongPolyLine(blueprint,
					buildData, verticesLine, heightFloorsOrder, unitDiameter, floorHeight,
					crossCountUnits, minCrossCountUnits, maxCrossCountUnits,
					1, 1, -1, unitCount, (maxUnitsRoof - 1), maxFloors, "hex", 0, 0, "inToOut");
				blueprint = blueprint1;
				buildData = buildData1;
			}
		}
		// TODO
		// name = prefab.GOName;
		// uniqueName = _lodash.GetInstanceId(name);
		// blueprint.land.game_objects.Add(uniqueName, new MultiplayerGameUserLandGameObjectClass(name,
		//	 pos, rot, scaleTemp, emptyTArray));
	}

	homePlotPaths += verticesInnerCircle;

	return {blueprint, homePlotPaths};
}
