#include "LMLobby.h"

#include "../LayoutModelBase.h"
#include "../LMRoomPlants.h"
#include "../LMTableChairs.h"
#include "../../Building/BuildingRoom.h"
#include "../../Common/DataConvert.h"
#include "../../Common/Lodash.h"
#include "../../Common/MathPolygon.h"
#include "../../Common/MathVector.h"
#include "../../Landscape/VerticesEdit.h"
#include "../../Modeling/ModelBase.h"
#include "../../Modeling/Furniture/ModelTable.h"
#include "../../Mesh/DynamicMaterial.h"
#include "../../Mesh/LoadContent.h"
#include "../../ModelingStructsActor.h"
#include "../../BuildingStructsActor.h"

LMLobby::LMLobby() {
}

LMLobby::~LMLobby() {
}

TMap<FString, FPolygon> LMLobby::CreateFromInputs() {
	ModelBase* modelBase = ModelBase::GetInstance();
	LayoutModelBase* layoutModelBase = LayoutModelBase::GetInstance();
	auto [layoutParams, modelParams] = layoutModelBase->GetInputs("Lobby", FVector(10,12,4));
	FString name = layoutParams.name;
	FVector size = layoutParams.size;
	return Create(size, modelParams, FModelCreateParams());
}

TMap<FString, FPolygon> LMLobby::Create(FVector size, FModelParams modelParams,
	FModelCreateParams createParamsIn) {
	FVector rotation = FVector(0,0,0), location = FVector(0,0,0), scale = FVector(1,1,1);
	FString uName, type, pairsString, scaleString;
	TArray<FVector> vertices;
	TMap<FString, FPolygon> polygons = {};
	uName = Lodash::GetInstanceId("Room");
	pairsString = "meshRule=roomCube&mat=white&bottomMat=marbleTile&scale=" + DataConvert::VectorToString(size) +
		ModelBase::AddRotationString(createParamsIn.rotation);
	vertices = { createParamsIn.offset };
	vertices = ModelBase::Vertices(vertices, createParamsIn);
	polygons.Add(uName, FPolygon(uName, uName, vertices, FVector(0,0,0), "room", "point", pairsString));

	FWallPlanterBox plantParams;
	plantParams.pairsStringPlants = "meshes=brackenFern,solidFern,cinnamonFern&placeOffsetAverage=0.5";
	LMRoomPlants::WallPlanterBox(size, modelParams, createParamsIn, plantParams);

	// Lights

	scale = FVector(size.X / 2, size.Y / 2, 1);
	LMLobby::CouchesCoffeeTables(scale, modelParams, createParamsIn);

	VerticesEdit* verticesEdit = VerticesEdit::GetInstance();
	verticesEdit->AddAndSave(polygons);
	return polygons;
}

TMap<FString, FPolygon> LMLobby::TwoTables(FVector size, FModelParams modelParams,
	FModelCreateParams createParamsIn) {
	FVector rotation = FVector(0,0,0), location = FVector(0,0,0), scale = FVector(1,1,1);
	FString uName, type, pairsString, scaleString;
	TArray<FVector> vertices;
	TMap<FString, FPolygon> polygons = {};
	uName = Lodash::GetInstanceId("Room");
	pairsString = "meshRule=roomCube&mat=white&bottomMat=marbleTile&scale=" + DataConvert::VectorToString(size) +
		ModelBase::AddRotationString(createParamsIn.rotation);
	vertices = { createParamsIn.offset };
	vertices = ModelBase::Vertices(vertices, createParamsIn);
	polygons.Add(uName, FPolygon(uName, uName, vertices, FVector(0,0,0), "room", "point", pairsString));

	FWallPlants plantParams;
	plantParams.pairsStringPlants = "meshes=brackenFern,solidFern,cinnamonFern&placeOffsetAverage=0.3";
	plantParams.walls = { "front", "back" };
	plantParams.zOffset = 0;
	plantParams.sideOffset = 0;
	LMRoomPlants::WallPlants(size, modelParams, createParamsIn, plantParams);

	scale = FVector(1.5 + 1 * 2, 1.5 + 1 * 2, 1);
	FTableChairs tableParams;
	tableParams.meshesByTags["table"] = { "shortRoundTableWood" };
	tableParams.meshesByTags["chair"] = { "chairTeal" };
	tableParams.offset = FVector(-2.5,-3,0);
	// FModelCreateParams createParamsTemp;
	// createParamsTemp.rotation = createParamsIn.rotation;
	// createParamsTemp.offset = createParamsIn.offset + FVector(-3.5,-5,0);
	LMTableChairs::TableWithChairs(scale, modelParams, createParamsIn, tableParams);

	scale = FVector(4.5 + 1 * 1.5, 3 + 1 * 2, 1);
	tableParams.meshesByTags["table"] = { "tableGlassLine" };
	tableParams.meshesByTags["chair"] = { "chair" };
	tableParams.offset = FVector(1,3,0);
	// createParamsTemp.offset = createParamsIn.offset + FVector(1,4,0);
	LMTableChairs::TableWithChairs(scale, modelParams, createParamsIn, tableParams);

	VerticesEdit* verticesEdit = VerticesEdit::GetInstance();
	verticesEdit->AddAndSave(polygons);
	return polygons;
}

TMap<FString, FPolygon> LMLobby::CouchesCoffeeTables(FVector size, FModelParams modelParams,
	FModelCreateParams createParamsIn) {
	FString uName, type, pairsString;
	TArray<FVector> vertices;
	TMap<FString, FPolygon> polygons = {};

	uName = Lodash::GetInstanceId("Couch");
	pairsString = "mesh=couch2Cushions&dMat=leather&dMatColor=beige" +
		ModelBase::AddRotationString(createParamsIn.rotation, FVector(0,0,90));
	vertices = { MathVector::RotateVector(FVector(0,-3,0), createParamsIn.rotation) + createParamsIn.offset };
	vertices = ModelBase::Vertices(vertices, createParamsIn);
	polygons.Add(uName, FPolygon(uName, uName, vertices, FVector(0,0,0), "couch", "point", pairsString));

	uName = Lodash::GetInstanceId("Couch");
	pairsString = "mesh=couch3Cushions&dMat=leather&dMatColor=beige" +
		ModelBase::AddRotationString(createParamsIn.rotation);
	vertices = { MathVector::RotateVector(FVector(-3,0,0), createParamsIn.rotation) + createParamsIn.offset };
	vertices = ModelBase::Vertices(vertices, createParamsIn);
	polygons.Add(uName, FPolygon(uName, uName, vertices, FVector(0,0,0), "couch", "point", pairsString));

	uName = Lodash::GetInstanceId("Table");
	pairsString = "meshRule=tableRoundShort&scale=1.3,1.3,0.7" +
		ModelBase::AddRotationString(createParamsIn.rotation);
	vertices = { MathVector::RotateVector(FVector(0,0,0), createParamsIn.rotation) + createParamsIn.offset };
	vertices = ModelBase::Vertices(vertices, createParamsIn);
	polygons.Add(uName, FPolygon(uName, uName, vertices, FVector(0,0,0), "table", "point", pairsString));
	uName = Lodash::GetInstanceId("Table");
	pairsString = "meshRule=tableRoundShort&scale=0.6,0.6,0.7" +
		ModelBase::AddRotationString(createParamsIn.rotation);
	vertices = { MathVector::RotateVector(FVector(-1,-1,0), createParamsIn.rotation) + createParamsIn.offset };
	vertices = ModelBase::Vertices(vertices, createParamsIn);
	polygons.Add(uName, FPolygon(uName, uName, vertices, FVector(0,0,0), "table", "point", pairsString));
	uName = Lodash::GetInstanceId("Table");
	pairsString = "meshRule=tableRoundShort&scale=0.6,0.6,0.7" +
		ModelBase::AddRotationString(createParamsIn.rotation);
	vertices = { MathVector::RotateVector(FVector(-0.75,1.5,0), createParamsIn.rotation) + createParamsIn.offset };
	polygons.Add(uName, FPolygon(uName, uName, vertices, FVector(0,0,0), "table", "point", pairsString));

	uName = Lodash::GetInstanceId("Chair");
	pairsString = "mesh=chair" + 
		ModelBase::AddRotationString(createParamsIn.rotation, FVector(0,0,-115));
	vertices = { MathVector::RotateVector(FVector(0.5,2.5,0), createParamsIn.rotation) + createParamsIn.offset };
	vertices = ModelBase::Vertices(vertices, createParamsIn);
	polygons.Add(uName, FPolygon(uName, uName, vertices, FVector(0,0,0), "chair", "point", pairsString));
	uName = Lodash::GetInstanceId("Chair");
	pairsString = "mesh=chair" + 
		ModelBase::AddRotationString(createParamsIn.rotation, FVector(0,0,-145));
	vertices = { MathVector::RotateVector(FVector(1.5,1.5,0), createParamsIn.rotation) + createParamsIn.offset };
	vertices = ModelBase::Vertices(vertices, createParamsIn);
	polygons.Add(uName, FPolygon(uName, uName, vertices, FVector(0,0,0), "chair", "point", pairsString));

	VerticesEdit* verticesEdit = VerticesEdit::GetInstance();
	verticesEdit->AddAndSave(polygons);
	return polygons;
}
