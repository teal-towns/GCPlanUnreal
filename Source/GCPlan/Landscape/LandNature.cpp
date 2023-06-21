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
	TArray<FString> meshNames = loadContent->GetMeshNamesByTypes({ "tree" });
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

	placeParams.offsetAverage = 30;
	for (auto& Elem : polygonsNature) {
		layoutPolygon->PlaceInPolygon(Elem.Value.vertices, meshNames, placeParams);
	}
	meshNames = loadContent->GetMeshNamesByTypes({ "bush" });
	// placeParams.snapToGround = true;
	placeParams.offsetAverage = 10;
	for (auto& Elem : polygonsNature) {
		layoutPolygon->PlaceInPolygon(Elem.Value.vertices, meshNames, placeParams);
	}
}
