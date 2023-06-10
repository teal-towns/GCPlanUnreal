#include "LandNature.h"

#include "../BuildingStructsActor.h"
#include "../Common/Lodash.h"
#include "../Common/MathPolygon.h"
#include "../Common/UnrealGlobal.h"
#include "../Data/DataFileProject.h"
#include "../Layout/LayoutPlace.h"
#include "../Layout/LayoutPolygon.h"
#include "../Layout/LayoutPolyLine.h"
#include "../Mesh/LoadContent.h"

LandNature::LandNature() {
}

LandNature::~LandNature() {
}

void LandNature::PlaceNature(TMap<FString, FPlot> plots) {
	UnrealGlobal* unrealGlobal = UnrealGlobal::GetInstance();
	LayoutPolygon* layoutPolygon = LayoutPolygon::GetInstance();
	LoadContent* loadContent = LoadContent::GetInstance();
	// Use another json file for the land polygon.
	TMap<FString, FPlot> landPlots = {};
	TMap<FString, FString> jsonFiles = unrealGlobal->_settings->projectJsonFiles;
	auto [data1, valid1] = DataFileProject::LoadProject(jsonFiles["landNature"]);
	// if (false) {
	if (valid1) {
		landPlots = data1.plots;
	} else {
		float z = 0;
		landPlots = {
			{ "marinaVillage", {"id6", "plot6",
				{ FVector(114, -444, z), FVector(-182, -187, z), FVector(-284, 414, z), FVector(524, 421, z) },
				FVector(0,0,z), "flowerHomes", 150 }
			},
		};
	}
	FVector center;
	TArray<FString> meshNames = loadContent->GetMeshNamesByTypes({ "tree" });
	FPlaceParams placeParams = FPlaceParams();
	placeParams.snapToGround = true;
	// Skip plots.
	for (auto& Elem : plots) {
		// // Only add final plots (not parent plots).
		// if (Elem.Value.childPlotUNames.Num() < 1) {
		// 	placeParams.skipPolygons.Add(Elem.Value.vertices);
		// }
		// Only add parent plots.
		if (Elem.Value.parentPlotUName == "") {
			placeParams.skipPolygons.Add(Elem.Value.vertices);
		}
	}

	placeParams.offsetAverage = 30;
	for (auto& Elem : landPlots) {
		center = MathPolygon::GetPolygonCenter(Elem.Value.vertices);
		layoutPolygon->PlaceInPolygon(Elem.Value.vertices, meshNames, center, placeParams);
	}
	meshNames = loadContent->GetMeshNamesByTypes({ "bush" });
	// placeParams.snapToGround = true;
	placeParams.offsetAverage = 10;
	for (auto& Elem : landPlots) {
		center = MathPolygon::GetPolygonCenter(Elem.Value.vertices);
		layoutPolygon->PlaceInPolygon(Elem.Value.vertices, meshNames, center, placeParams);
	}
}
