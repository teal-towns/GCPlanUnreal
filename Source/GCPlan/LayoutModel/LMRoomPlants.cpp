#include "LMRoomPlants.h"

#include "Engine/StaticMeshActor.h"

#include "LayoutModelBase.h"
#include "../Common/DataConvert.h"
#include "../Common/Lodash.h"
#include "../Common/MathPolygon.h"
#include "../Common/MathVector.h"
#include "../Landscape/VerticesEdit.h"
#include "../Layout/LayoutPlace.h"
#include "../Layout/LayoutPolygon.h"
#include "../Mesh/LoadContent.h"
#include "../Modeling/ModelBase.h"
#include "../ModelingStructsActor.h"
#include "../BuildingStructsActor.h"

LMRoomPlants::LMRoomPlants() {
}

LMRoomPlants::~LMRoomPlants() {
}

TMap<FString, FPolygon> LMRoomPlants::WallPlanterBox(FVector size, FModelParams modelParams,
	FModelCreateParams createParamsIn, FWallPlanterBox params) {
	FVector rotation = FVector(0,0,0), location = FVector(0,0,0), scale = FVector(1,1,1);
	FString uName, type, pairsString, scaleString, wall;
	TArray<FVector> vertices;
	TMap<FString, FPolygon> polygons = {};
	for (int ii = 0; ii < params.walls.Num(); ii++) {
		wall = params.walls[ii];
		if (wall == "left" || wall == "right") {
			scale = FVector(size.X - params.wallOffsetLength * 2, params.width, params.height);
			if (wall == "left") {
				location = FVector(0, size.Y / -2 + params.wallOffsetWidth, 0);
			} else {
				location = FVector(0, size.Y / 2 - params.wallOffsetWidth, 0);
			}
		} else {
			scale = FVector(params.width, size.Y - params.wallOffsetLength * 2, params.height);
			if (wall == "back") {
				location = FVector(size.X / -2 + params.wallOffsetWidth, 0, 0);
			} else {
				location = FVector(size.X / 2 - params.wallOffsetWidth, 0, 0);
			}
		}
		params.offset = location;
		MultiPlantsBox(scale, modelParams, createParamsIn, params);
		// uName = Lodash::GetInstanceId("PlanterBox");
		// pairsString = "meshRule=planterBox&scale=" + DataConvert::VectorToString(scale) +
		// 	ModelBase::AddRotationString(createParamsIn.rotation);
		// vertices = { MathVector::RotateVector(location, createParamsIn.rotation) + createParamsIn.offset };
		// polygons.Add(uName, FPolygon(uName, uName, vertices, FVector(0,0,0), "planterBox", "point", pairsString));
		// // plants
		// location += FVector(0,0,params.height);
		// vertices = MathVector::RotateVertices(MathPolygon::PointToBox(location, scale), createParamsIn.rotation,
		// 	createParamsIn.offset);
		// uName = Lodash::GetInstanceId("Plants");
		// pairsString = params.pairsStringPlants;
		// polygons.Add(uName, FPolygon(uName, uName, vertices, FVector(0,0,0), "bush", "polygon", pairsString));
	}

	VerticesEdit* verticesEdit = VerticesEdit::GetInstance();
	verticesEdit->AddAndSave(polygons);
	return polygons;
}

TMap<FString, FPolygon> LMRoomPlants::MultiPlantsBox(FVector size, FModelParams modelParams,
	FModelCreateParams createParamsIn, FWallPlanterBox params) {
	FVector rotation = FVector(0,0,0), location = FVector(0,0,0), scale = FVector(1,1,1);
	FString uName, type, pairsString, scaleString, wall;
	TArray<FVector> vertices;
	TMap<FString, FPolygon> polygons = {};
	FString uNameBase = Lodash::GetInstanceId("multiPlants");

	location = FVector(0,0,0);
	scale = FVector(size.X, size.Y, params.height);
	uName = uNameBase + "_box";
	pairsString = "meshRule=planterBox&scale=" + DataConvert::VectorToString(scale) +
		ModelBase::AddRotationString(createParamsIn.rotation);
	vertices = { MathVector::RotateVector(location + params.offset, createParamsIn.rotation) + createParamsIn.offset };
	polygons.Add(uName, FPolygon(uName, uName, vertices, FVector(0,0,0), "planterBox", "point", pairsString));
	// plants
	location += FVector(0,0,params.height);
	vertices = MathVector::RotateVertices(MathPolygon::PointToBox(location + params.offset, scale), createParamsIn.rotation,
		createParamsIn.offset);
	uName = uNameBase + "_plants";
	pairsString = params.pairsStringPlants;
	polygons.Add(uName, FPolygon(uName, uName, vertices, FVector(0,0,0), "bush", "polygon", pairsString));

	VerticesEdit* verticesEdit = VerticesEdit::GetInstance();
	verticesEdit->AddAndSave(polygons);
	return polygons;
}

TMap<FString, FPolygon> LMRoomPlants::PlanterBox(FVector size, FModelParams modelParams,
	FModelCreateParams createParamsIn, FPlanterBox params) {
	FVector rotation = FVector(0,0,0), location = FVector(0,0,0), scale = FVector(1,1,1);
	FString uName, type, pairsString, scaleString, wall;
	TArray<FVector> vertices;
	TMap<FString, FPolygon> polygons = {};
	LoadContent* loadContent = LoadContent::GetInstance();
	FString uNameBase = Lodash::GetInstanceId("planterBox");

	auto [valid, meshesByTags] = loadContent->FillMeshesByTags(params.meshesByTags, { "indoorBush" });
	if (!valid) {
		UE_LOG(LogTemp, Warning, TEXT("LMRoomPlants.PlanterBox missing meshesByTags, skipping"));
		return polygons;
	}
	params.meshesByTags = meshesByTags;

	uName = uNameBase + "_planterBox";
	scale = FVector(size.X, size.Y, params.planterHeight);
	location = FVector(0,0,0);
	pairsString = "meshRule=" + params.meshRule + "&scale=" + DataConvert::VectorToString(scale) +
		ModelBase::AddRotationString(createParamsIn.rotation);
	vertices = { MathVector::RotateVector(location + params.offset, createParamsIn.rotation) + createParamsIn.offset };
	vertices = ModelBase::Vertices(vertices, createParamsIn, params.rotation);
	polygons.Add(uName, FPolygon(uName, uName, vertices, FVector(0,0,0), "planterBox", "point", pairsString));

	uName = uNameBase + "_plant";
	location.Z = params.planterHeight;
	FString meshKey = params.meshesByTags["indoorBush"][Lodash::RandomRangeInt(0, params.meshesByTags["indoorBush"].Num() - 1)];
	pairsString = "mesh=" + meshKey + ModelBase::AddRotationString(createParamsIn.rotation);
	vertices = { MathVector::RotateVector(location + params.offset, createParamsIn.rotation) + createParamsIn.offset };
	vertices = ModelBase::Vertices(vertices, createParamsIn, params.rotation);
	polygons.Add(uName, FPolygon(uName, uName, vertices, FVector(0,0,0), "bush", "point", pairsString));

	VerticesEdit* verticesEdit = VerticesEdit::GetInstance();
	verticesEdit->AddAndSave(polygons);
	return polygons;
}

TMap<FString, FPolygon> LMRoomPlants::WallPlants(FVector size, FModelParams modelParams,
	FModelCreateParams createParamsIn, FWallPlants params) {
	FVector rotation = FVector(0,0,0), location = FVector(0,0,0), scale = FVector(1,1,1);
	FString uName, type, pairsString, scaleString, wall;
	TArray<FVector> vertices;
	TMap<FString, FPolygon> polygons = {};
	for (int ii = 0; ii < params.walls.Num(); ii++) {
		wall = params.walls[ii];
		// Note: z location will be centered for non xy plane (usually z offset is based on bottom / ground pivot).
		if (wall == "left" || wall == "right") {
			scale = FVector(size.X - params.sideOffset * 2, params.width, size.Z - params.zOffset * 2);
			if (wall == "left") {
				location = FVector(0, size.Y / -2 + scale.Y / 2, size.Z / 2);
				pairsString = params.pairsStringPlants + "&placeRotMinX=85&placeRotMaxX=95&placePlane=xz";
			} else {
				location = FVector(0, size.Y / 2 - scale.Y / 2, size.Z / 2);
				pairsString = params.pairsStringPlants + "&placeRotMinX=265&placeRotMaxX=275&placePlane=xz";
			}
			vertices = MathVector::RotateVertices(MathPolygon::PointToBox(location, scale, "xz"), createParamsIn.rotation,
				createParamsIn.offset);
		} else {
			scale = FVector(params.width, size.Y - params.sideOffset * 2, size.Z - params.zOffset * 2);
			if (wall == "back") {
				location = FVector(size.X / -2 + scale.X / 2, 0, size.Z / 2);
				pairsString = params.pairsStringPlants + "&placeRotMinY=265&placeRotMaxY=275&placePlane=yz";
			} else {
				location = FVector(size.X / 2 - scale.X / 2, 0, size.Z / 2);
				pairsString = params.pairsStringPlants + "&placeRotMinY=85&placeRotMaxY=95&placePlane=yz";
			}
			vertices = MathVector::RotateVertices(MathPolygon::PointToBox(location, scale, "yz"), createParamsIn.rotation,
				createParamsIn.offset);
		}
		// plants
		uName = Lodash::GetInstanceId("wallPlants");
		polygons.Add(uName, FPolygon(uName, uName, vertices, FVector(0,0,0), "bush", "polygon", pairsString));
	}

	VerticesEdit* verticesEdit = VerticesEdit::GetInstance();
	verticesEdit->AddAndSave(polygons);
	return polygons;
}

TMap<FString, FPolygon> LMRoomPlants::HangingPlants(FVector size, FModelParams modelParams,
	FModelCreateParams createParamsIn, FHangingPlants params) {
	FVector rotation = FVector(0,0,0), location = FVector(0,0,0), scale = FVector(1,1,1);
	FString uName, type, pairsString, scaleString, meshKey;
	TArray<FVector> vertices;
	TMap<FString, FPolygon> polygons = {};
	FString uNameBase = Lodash::GetInstanceId("hangingPlants");
	LoadContent* loadContent = LoadContent::GetInstance();
	auto [valid, meshesByTags] = loadContent->FillMeshesByTags(params.meshesByTags);
	if (!valid) {
		UE_LOG(LogTemp, Warning, TEXT("LMRoomPlants.HangingPlants missing meshesByTags, skipping"));
		return polygons;
	}
	params.meshesByTags = meshesByTags;

	FPlaceParams placeParams;
	placeParams.skipMesh = true;
	placeParams.offsetAverage = params.placeOffset;
	placeParams.offsetMaxFactorX = params.placeOffsetMaxYFactor;
	placeParams.offsetMaxFactorY = params.placeOffsetMaxYFactor;
	vertices = MathPolygon::PointToBox(FVector(0,0,0), size);
	TMap<FString, FMeshTransform> transforms = LayoutPolygon::PlaceInPolygon(vertices, {}, placeParams);
	int counter = 0;
	float currentZ;
	for (auto& Elem : transforms) {
		FVector locationBase = Elem.Value.location;
		currentZ = size.Z;

		uName = uNameBase + "_" + FString::FromInt(counter) + "_cord";
		scale = params.cordScale;
		location = FVector(locationBase.X, locationBase.Y, currentZ - scale.Z);
		pairsString = "meshRule=cord&scale=" + DataConvert::VectorToString(scale) +
			ModelBase::AddRotationString(createParamsIn.rotation);
		vertices = { MathVector::RotateVector(location + params.offset, createParamsIn.rotation) + createParamsIn.offset };
		vertices = ModelBase::Vertices(vertices, createParamsIn, params.rotation);
		polygons.Add(uName, FPolygon(uName, uName, vertices, FVector(0,0,0), "cord", "point", pairsString));
		currentZ -= scale.Z;

		uName = uNameBase + "_" + FString::FromInt(counter) + "_plant";
		location = FVector(locationBase.X, locationBase.Y, currentZ);
		meshKey = params.meshesByTags["hangingPlant"][Lodash::RandomRangeInt(0, params.meshesByTags["hangingPlant"].Num() - 1)];
		pairsString = "mesh=" + meshKey + ModelBase::AddRotationString(createParamsIn.rotation);
		vertices = { MathVector::RotateVector(location + params.offset, createParamsIn.rotation) + createParamsIn.offset };
		vertices = ModelBase::Vertices(vertices, createParamsIn, params.rotation);
		polygons.Add(uName, FPolygon(uName, uName, vertices, FVector(0,0,0), "bush", "point", pairsString));

		uName = uNameBase + "_" + FString::FromInt(counter) + "_flowerPot";
		scale = params.potScale;
		location = FVector(locationBase.X, locationBase.Y, currentZ - scale.Z / 2);
		meshKey = params.meshesByTags["flowerPot"][Lodash::RandomRangeInt(0, params.meshesByTags["flowerPot"].Num() - 1)];
		pairsString = "mesh=" + meshKey + "&scale=" + loadContent->MeshScale(scale, meshKey) +
			ModelBase::AddRotationString(createParamsIn.rotation);
		vertices = { MathVector::RotateVector(location + params.offset, createParamsIn.rotation) + createParamsIn.offset };
		vertices = ModelBase::Vertices(vertices, createParamsIn, params.rotation);
		polygons.Add(uName, FPolygon(uName, uName, vertices, FVector(0,0,0), "flowerPot", "point", pairsString));
		currentZ -= scale.Z;

		counter += 1;
	}

	VerticesEdit* verticesEdit = VerticesEdit::GetInstance();
	verticesEdit->AddAndSave(polygons);
	return polygons;
}
