#include "PlotBuild.h"

#include "../BuildingStructsActor.h"
#include "../Building/BuildingFlowerHomes.h"
#include "../Building/BuildingRing.h"
#include "../Common/DataConvert.h"
#include "../Common/UnrealGlobal.h"
#include "../Mesh/InstancedMesh.h"

PlotBuild::PlotBuild() {
}

PlotBuild::~PlotBuild() {
}

TArray<FLand> PlotBuild::CreateLands(TMap<FString, FPlot> plots) {
	TArray<FLand> lands = {};
	FBuildingBlueprint blueprint;
	FString pattern;
	FPlot plotTemp;
	for (auto& Elem : plots) {
		plotTemp = Elem.Value;
		// Only build if a final plot (no children).
		if (Elem.Value.childPlotUNames.Num() == 0) {
			pattern = Elem.Value.buildPattern;
			// TODO - add linked instructions per build pattern (one for each plot) and use those.
			if (pattern == "flowerHomes") {
				auto [blueprint1, homePlotPaths] = BuildingFlowerHomes::Create(plotTemp.vertices, plotTemp.verticesBuffer, 0.15, 2000, "outer");
				blueprint = blueprint1;
				// TODO? Or leave these out?
				// meshTerrain->AddRoads(homePlotPaths);
			} else {
				TArray<int> heightFloorsOrder = { 2, 10, 4, 7 };
				blueprint = BuildingRing::Create(plotTemp.vertices, heightFloorsOrder);
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
		UE_LOG(LogTemp, Display, TEXT("land_id %s num GO %d"), *lands[ii].land_id, lands[ii].game_objects.Num());
		for (auto& Elem : lands[ii].game_objects) {
			FLandGameObject GO = Elem.Value;
			instancedMesh->CreateInstance("HexModule", DataConvert::DictToVector(GO.position),
				DataConvert::DictToRotator(GO.rotation), DataConvert::DictToVector(GO.scale));
		}
	}
}
