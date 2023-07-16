#include "LandNature.h"

#include "../BuildingStructsActor.h"
#include "../Common/Lodash.h"
#include "../Common/MathPolygon.h"
#include "../Common/UnrealGlobal.h"
#include "../Data/DataFileProject.h"
#include "../Landscape/VerticesEdit.h"
#include "../Layout/LayoutPlace.h"
#include "../Layout/LayoutPolygon.h"
#include "../Layout/LayoutPolyLine.h"
#include "../Mesh/LoadContent.h"
#include "../ProceduralModel/PMPlanePolygon.h"

LandNature::LandNature() {
}

LandNature::~LandNature() {
}

void LandNature::PlaceNature() {
	UnrealGlobal* unrealGlobal = UnrealGlobal::GetInstance();
	LayoutPolygon* layoutPolygon = LayoutPolygon::GetInstance();
	LoadContent* loadContent = LoadContent::GetInstance();
	VerticesEdit* verticesEdit = VerticesEdit::GetInstance();

	TMap<FString, FPolygon> polygonsNature = verticesEdit->FilterByTypes({ "landNature" });
	TMap<FString, FPolygon> polygonsPlots = verticesEdit->FilterByTypes({ "plot" });

	FVector center;
	FPlaceParams placeParams = FPlaceParams();
	placeParams.snapToGround = true;
	// Skip plots.
	for (auto& Elem : polygonsPlots) {
		// Only add final ones (not parents).
		if (Elem.Value.childUNames.Num() < 1) {
			placeParams.skipPolygons.Add(Elem.Value.vertices);
		}
		// Only add parents.
		// if (Elem.Value.parentUName == "") {
		// 	placeParams.skipPolygons.Add(Elem.Value.vertices);
		// }
	}

	TArray<FString> meshNamesTree = loadContent->GetMeshNamesByTags({ "tree" });
	TArray<FString> meshNamesBush = loadContent->GetMeshNamesByTags({ "outdoorBush" });
	TArray<FString> meshNamesGrass = loadContent->GetMeshNamesByTags({ "grass" });
	// TArray<FString> meshNamesGrassPlane = loadContent->GetMeshNamesByTags({ "grassPlane" });
	// placeParams.snapToGround = true;

	// FPlaceParams placeParamsPlane = FPlaceParams();
	// placeParamsPlane.snapToGround = true;
	// float scale = 5;
	// placeParamsPlane.scaleMin = scale;
	// placeParamsPlane.scaleMax = scale;
	// placeParamsPlane.offsetX = scale;
	// placeParamsPlane.offsetY = scale;
	// placeParamsPlane.offsetMaxFactorX = 0;
	// placeParamsPlane.offsetMaxFactorY = 0;
	// placeParamsPlane.offsetMaxFactorZ = 0;
	// placeParamsPlane.rotMaxZ = 0;
	FModelParams modelParams;
	FModelCreateParams createParams;
	FPlanePolygon planeParams;

	TMap<FString, FString> pairs;
	for (auto& Elem : polygonsNature) {
		pairs = Lodash::PairsStringToObject(Elem.Value.pairsString);
		placeParams.offsetAverage = 15;
		placeParams.scaleMin = 0.33;
		placeParams.scaleMax = 0.75;
		LayoutPolygon::PlaceInPolygon(Elem.Value.vertices, meshNamesTree, placeParams);
		if (pairs.Contains("bush")) {
			placeParams.offsetAverage = 5;
			placeParams.scaleMin = 0.75;
			placeParams.scaleMax = 1.25;
			LayoutPolygon::PlaceInPolygon(Elem.Value.vertices, meshNamesBush, placeParams);
		}
		if (pairs.Contains("grass")) {
			placeParams.offsetAverage = 5;
			LayoutPolygon::PlaceInPolygon(Elem.Value.vertices, meshNamesGrass, placeParams);
		} else if (pairs.Contains("grassPlaneMaterialKey")) {
			// LayoutPolygon::PlaceInPolygon(Elem.Value.vertices, meshNamesGrassPlane, placeParamsPlane);
			createParams.offset = FVector(0,0,0);
			modelParams.materialKey = pairs["grassPlaneMaterialKey"];
			PMPlanePolygon::Create(Elem.Value.vertices, createParams, modelParams, planeParams);
		}
	}

	TMap<FString, FPolygon> polygons = verticesEdit->FilterByTypes({ "grass" });
	planeParams.triangleDirection = "counterClockwise";
	for (auto& Elem : polygons) {
		pairs = Lodash::PairsStringToObject(Elem.Value.pairsString);
		createParams.offset = FVector(0,0,0);
		modelParams.materialKey = pairs.Contains("grassPlaneMaterialKey") ? pairs["grassPlaneMaterialKey"] : "grass";
		// UE_LOG(LogTemp, Display, TEXT("grass %s"), *Elem.Key);
		PMPlanePolygon::Create(Elem.Value.vertices, createParams, modelParams, planeParams);
	}
}
