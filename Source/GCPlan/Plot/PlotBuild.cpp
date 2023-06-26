#include "PlotBuild.h"

#include "../BuildingStructsActor.h"
#include "../Building/BuildingFlowerHomes.h"
#include "../Building/BuildingRing.h"
#include "../Common/DataConvert.h"
#include "../Common/MathPolygon.h"
#include "../Common/UnrealGlobal.h"
#include "../Landscape/VerticesEdit.h"
#include "../Layout/LayoutPlace.h"
#include "../Layout/LayoutPolygon.h"
#include "../Layout/LayoutPolyLine.h"
#include "../Mesh/InstancedMesh.h"
#include "../Mesh/LoadContent.h"

PlotBuild::PlotBuild() {
}

PlotBuild::~PlotBuild() {
}

TArray<FLand> PlotBuild::CreateLands(TMap<FString, FPolygon> polygons, float unitDiameter) {
	VerticesEdit* verticesEdit = VerticesEdit::GetInstance();
	TArray<FLand> lands = {};
	FBuildingBlueprint blueprint;
	FString pairsString;
	FPolygon polygonTemp;
	for (auto& Elem : polygons) {
		polygonTemp = Elem.Value;
		// Only build if a final one (no children).
		if (Elem.Value.childUNames.Num() == 0) {
			pairsString = Elem.Value.pairsString;
			// If empty, use parent.
			pairsString = verticesEdit->GetParentPairsString("plot", Elem.Key);
			if (pairsString.Contains("buildPattern=flowerHomes")) {
				float verticesBuffer = polygonTemp.verticesBuffer;
				// For plants instead of roads, do NOT go all the way (onto road).
				verticesBuffer = -5;
				auto [blueprint1, homePlotPaths] = BuildingFlowerHomes::Create(polygonTemp.vertices, verticesBuffer, 0.15, 2000, "outer");
				blueprint = blueprint1;
				// meshTerrain->AddRoads(homePlotPaths);
				FlowerHomePlants(homePlotPaths);
			} else {
				TArray<int> heightFloorsOrder = { 2, 10, 4, 7 };
				blueprint = BuildingRing::Create(polygonTemp.vertices, heightFloorsOrder);
				// TODO - un hardcode
				int crossUnitsCount = 3;
				RingPlantsCenter(polygonTemp.vertices, unitDiameter, crossUnitsCount);
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
			// instancedMesh->CreateInstance("HexModuleBlock", DataConvert::StringToVector(GO.position),
			// 	DataConvert::StringToRotator(GO.rotation), DataConvert::StringToVector(GO.scale));
			instancedMesh->CreateInstance("HexModuleBlock", GO.position, GO.rotation, GO.scale);
		}
	}
}

void PlotBuild::FlowerHomePlants(TMap<FString, FPolygon> homePlotPaths) {
	TArray<FVector> vertices;
	LoadContent* loadContent = LoadContent::GetInstance();
	TArray<FString> meshNamesBush = loadContent->GetMeshNamesByTypes({ "bush" });
	TArray<FString> meshNamesTree = loadContent->GetMeshNamesByTypes({ "tree" });
	FPlaceParams placeParams = FPlaceParams();
	placeParams.snapToGround = true;
	placeParams.width = 5;
	for (auto& Elem : homePlotPaths) {
		vertices = Elem.Value.vertices;

		placeParams.spacing = 2;
		placeParams.spacingCrossAxis = 2;
		placeParams.scaleMin = 0.75;
		placeParams.scaleMax = 1.25;
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
	LayoutPolygon::PlaceInPolygon(verticesInner, meshNames, placeParams, center);

	meshNames = loadContent->GetMeshNamesByTypes({ "bush" });
	placeParams.offsetAverage = 5;
	LayoutPolygon::PlaceInPolygon(verticesInner, meshNames, placeParams, center);
}
