#include "LMCafeteria.h"

#include "../LayoutModelBase.h"
#include "../LMKitchen.h"
#include "../LMRoomPlants.h"
#include "../LMTableChairs.h"
#include "../../Common/DataConvert.h"
#include "../../Common/Lodash.h"
#include "../../Common/MathPolygon.h"
#include "../../Common/MathVector.h"
#include "../../Landscape/VerticesEdit.h"
#include "../../Layout/LayoutPlace.h"
#include "../../Layout/LayoutPolyLine.h"
#include "../../Mesh/LoadContent.h"
#include "../../Modeling/ModelBase.h"
#include "../../Modeling/Furniture/ModelTable.h"
#include "../../Mesh/DynamicMaterial.h"
#include "../../Mesh/LoadContent.h"
#include "../../ModelingStructsActor.h"
#include "../../BuildingStructsActor.h"

LMCafeteria::LMCafeteria() {
}

LMCafeteria::~LMCafeteria() {
}

TMap<FString, FPolygon> LMCafeteria::Create(FVector size, FModelParams modelParams,
	FModelCreateParams createParamsIn) {
	FVector rotation = FVector(0,0,0), location = FVector(0,0,0), scale = FVector(1,1,1);
	FString uName, type, pairsString, scaleString, meshKey;
	TArray<FVector> vertices;
	TMap<FString, FPolygon> polygons = {};
	LoadContent* loadContent = LoadContent::GetInstance();
	FString uNameBase = Lodash::GetInstanceId("cafeteria");

	// uName = Lodash::GetInstanceId("Room");
	// pairsString = "meshRule=roomCube&mat=white&bottomMat=marbleTile&scale=" + DataConvert::VectorToString(size) +
	// 	ModelBase::AddRotationString(createParamsIn.rotation);
	// vertices = { createParamsIn.offset };
	// vertices = ModelBase::Vertices(vertices, createParamsIn);
	// polygons.Add(uName, FPolygon(uName, uName, vertices, FVector(0,0,0), "room", "point", pairsString));

	float planterBoxXScale = 0.6;
	// Planter box on one side, kitchen then planter box on other, tables in middle filling rest of space.
	FWallPlanterBox plantParams;
	FString meshes = Lodash::Join(loadContent->GetMeshNamesByTags({"indoorBush"}), ",");
	plantParams.width = planterBoxXScale;
	plantParams.pairsStringPlants = "meshes=" + meshes + "&placeOffsetAverage=0.3";
	plantParams.walls = { "back" };
	LMRoomPlants::WallPlanterBox(size, modelParams, createParamsIn, plantParams);

	// Kitchen bar in front, with plants on sides (in corner).
	FPlanterBox planterParams;
	planterParams.planterHeight = 1;
	scale = FVector(0.6, 0.6, planterParams.planterHeight);
	float buffer = 0.3;
	// front left
	planterParams.offset = FVector(size.X / 2 - scale.X / 2 - buffer, size.Y / -2 + scale.Y / 2 + buffer, 0);
	LMRoomPlants::PlanterBox(scale, modelParams, createParamsIn, planterParams);
	// front right
	planterParams.offset.Y = size.Y / 2 - scale.Y / 2 - buffer;
	LMRoomPlants::PlanterBox(scale, modelParams, createParamsIn, planterParams);

	FVector kitchenScale = FVector(3, size.Y - 2 * 2, size.Z);
	float currentX = size.X / 2;
	FKitchen kitchenParams;
	kitchenParams.offset = FVector(currentX - kitchenScale.X / 2, 0, 0);
	LMKitchen::OfficeWall(kitchenScale, modelParams, createParamsIn, kitchenParams);

	uName = uNameBase + "_personKitchen";
	location = FVector(currentX - 2, -2, 0);
	rotation = FVector(0,0,180);
	meshKey = "manStandingBag";
	pairsString = "mesh=" + meshKey + ModelBase::AddRotationString(createParamsIn.rotation, rotation, meshKey);
	vertices = { MathVector::RotateVector(location, createParamsIn.rotation) + createParamsIn.offset };
	vertices = ModelBase::Vertices(vertices, createParamsIn);
	polygons.Add(uName, FPolygon(uName, uName, vertices, FVector(0,0,0), "person", "point", pairsString));

	// hanging plants above kitchen
	FHangingPlants hangingParams;
	scale = kitchenScale - FVector(1, 0, 0);
	hangingParams.offset = FVector(currentX - scale.X / 2 - 1, 0, 0);
	hangingParams.placeOffset = 1.25;
	LMRoomPlants::HangingPlants(scale, modelParams, createParamsIn, hangingParams);

	// planter box in front of kitchen bar
	currentX -= kitchenScale.X;
	scale = FVector(planterBoxXScale, kitchenScale.Y, 1);
	plantParams.offset = FVector(currentX - scale.X / 2, 0, 0);
	plantParams.height = scale.Z;
	LMRoomPlants::MultiPlantsBox(scale, modelParams, createParamsIn, plantParams);

	// tables for rest of space.
	buffer = 2;
	scale = FVector(size.X - kitchenScale.X - planterBoxXScale * 3 - buffer, size.Y - buffer * 2, size.Z);
	location = FVector(currentX - buffer - 0.75, 0, 0);
	FTableChairs tableParams;
	tableParams.offset = location;
	tableParams.meshesByTags["table"] = { "tableRoundCenterLeg" };
	tableParams.meshesByTags["chair"] = { "chairWood" };
	tableParams.meshesByTags["people"] = { "manSittingLaptop", "womanSittingBook" };
	tableParams.chairPositions = { "left", "right" };
	tableParams.placeOffsetX = 1.75;
	LMTableChairs::TablesAndChairs(scale, modelParams, createParamsIn, tableParams);

	VerticesEdit* verticesEdit = VerticesEdit::GetInstance();
	verticesEdit->AddAndSave(polygons);
	return polygons;
}
