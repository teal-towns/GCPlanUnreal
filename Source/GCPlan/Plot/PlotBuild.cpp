#include "PlotBuild.h"

#include "../BuildingStructsActor.h"
#include "../Building/BuildingFlowerHomes.h"
#include "../Building/BuildingRing.h"
#include "../Common/DataConvert.h"
#include "../Common/MathPolygon.h"
#include "../Common/UnrealGlobal.h"
#include "../Layout/LayoutPlace.h"
#include "../Layout/LayoutPolygon.h"
#include "../Layout/LayoutPolyLine.h"
#include "../Mesh/InstancedMesh.h"
#include "../Mesh/LoadContent.h"
#include "../Plot/PlotData.h"

PlotBuild::PlotBuild() {
}

PlotBuild::~PlotBuild() {
}

TArray<FLand> PlotBuild::CreateLands(TMap<FString, FPlot> plots, float unitDiameter) {
	PlotData* plotData = PlotData::GetInstance();
	TArray<FLand> lands = {};
	FBuildingBlueprint blueprint;
	FString pattern;
	FPlot plotTemp;
	for (auto& Elem : plots) {
		plotTemp = Elem.Value;
		// Only build if a final plot (no children).
		if (Elem.Value.childPlotUNames.Num() == 0) {
			pattern = Elem.Value.buildPattern;
			// If empty, use parent.
			pattern = plotData->GetParentPattern(Elem.Key);
			// TODO - add linked instructions per build pattern (one for each plot) and use those.
			if (pattern == "flowerHomes") {
				float verticesBuffer = plotTemp.verticesBuffer;
				// For plants instead of roads, do NOT go all the way (onto road).
				verticesBuffer = -5;
				auto [blueprint1, homePlotPaths] = BuildingFlowerHomes::Create(plotTemp.vertices, verticesBuffer, 0.15, 2000, "outer");
				blueprint = blueprint1;
				// meshTerrain->AddRoads(homePlotPaths);
				FlowerHomePlants(homePlotPaths);
			} else {
				TArray<int> heightFloorsOrder = { 2, 10, 4, 7 };
				blueprint = BuildingRing::Create(plotTemp.vertices, heightFloorsOrder);
				// TODO - un hardcode
				int crossUnitsCount = 3;
				RingPlantsCenter(plotTemp.vertices, unitDiameter, crossUnitsCount);
			}
			lands.Add(blueprint.land);
		}
	}
	return lands;
}

void PlotBuild::DrawLands(TArray<FLand> lands) {
	UnrealGlobal* unrealGlobal = UnrealGlobal::GetInstance();
	InstancedMesh* instancedMesh = InstancedMesh::GetInstance();
	for (int ii = 0; ii < lands.Num(); ii++) {
		// UE_LOG(LogTemp, Display, TEXT("land_id %s num GO %d"), *lands[ii].land_id, lands[ii].game_objects.Num());
		for (auto& Elem : lands[ii].game_objects) {
			FLandGameObject GO = Elem.Value;
			instancedMesh->CreateInstance("HexModuleBlock", DataConvert::DictToVector(GO.position),
				DataConvert::DictToRotator(GO.rotation), DataConvert::DictToVector(GO.scale));
		}
	}
}

void PlotBuild::FlowerHomePlants(TMap<FString, FRoadPath> homePlotPaths) {
	float widthMeters;
	TArray<FVector> vertices;
	LoadContent* loadContent = LoadContent::GetInstance();
	TArray<FString> meshNamesBush = loadContent->GetMeshNamesByTypes({ "bush" });
	TArray<FString> meshNamesTree = loadContent->GetMeshNamesByTypes({ "tree" });
	FPlaceParams placeParams = FPlaceParams();
	placeParams.snapToGround = true;
	placeParams.width = 5;
	for (auto& Elem : homePlotPaths) {
		widthMeters = Elem.Value.widthMeters;
		vertices = Elem.Value.vertices;

		placeParams.spacing = 2;
		placeParams.spacingCrossAxis = 2;
		placeParams.scaleMin = 0.75;
		placeParams.scaleMax = 2;
		LayoutPolyLine::PlaceOnLine(vertices, meshNamesBush, placeParams);
		placeParams.spacing = 20;
		placeParams.spacingCrossAxis = 999;
		placeParams.scaleMin = 0.25;
		placeParams.scaleMax = 0.75;
		LayoutPolyLine::PlaceOnLine(vertices, meshNamesTree, placeParams);
	}
}

void PlotBuild::RingPlantsCenter(TArray<FVector> vertices, float unitDiameter, int crossUnitsCount) {
	FVector center = MathPolygon::GetPolygonCenter(vertices);
	TArray<FVector> verticesInner = MathPolygon::BufferVertices(vertices, center, -1 * unitDiameter * crossUnitsCount);
	TArray<FString> meshNames;
	FPlaceParams placeParams = FPlaceParams();
	placeParams.snapToGround = true;

	LoadContent* loadContent = LoadContent::GetInstance();
	LayoutPolygon* layoutPolygon = LayoutPolygon::GetInstance();

	meshNames = loadContent->GetMeshNamesByTypes({ "tree" });
	placeParams.offsetAverage = 15;
	layoutPolygon->PlaceInPolygon(verticesInner, meshNames, center, placeParams);

	meshNames = loadContent->GetMeshNamesByTypes({ "bush" });
	placeParams.offsetAverage = 5;
	layoutPolygon->PlaceInPolygon(verticesInner, meshNames, center, placeParams);
}
