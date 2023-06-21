#include "LMLobby.h"

#include "Engine/StaticMeshActor.h"

#include "LayoutModelBase.h"
#include "../Building/BuildingRoom.h"
#include "../Common/DataConvert.h"
#include "../Common/Lodash.h"
#include "../Common/MathPolygon.h"
#include "../Common/MathVector.h"
#include "../Landscape/VerticesEdit.h"
#include "../Modeling/ModelBase.h"
#include "../Modeling/Furniture/ModelTable.h"
#include "../Mesh/DynamicMaterial.h"
#include "../Mesh/LoadContent.h"
#include "../ModelingStructsActor.h"
#include "../BuildingStructsActor.h"

LMLobby::LMLobby() {
}

LMLobby::~LMLobby() {
}

TMap<FString, FPolygon> LMLobby::CreateFromInputs() {
	ModelBase* modelBase = ModelBase::GetInstance();
	LayoutModelBase* layoutModelBase = LayoutModelBase::GetInstance();
	UWorld* World = layoutModelBase->GetWorld();
	auto [layoutParams, modelParams] = layoutModelBase->GetInputs("Lobby", FVector(10,12,4));
	FString name = layoutParams.name;
	FVector size = layoutParams.size;
	TArray<FString> tags = layoutParams.tags;
	return Create(size, modelParams, FModelCreateParams(), name);
}

TMap<FString, FPolygon> LMLobby::Create(FVector size, FModelParams modelParams,
	FModelCreateParams createParamsIn, FString name) {
	ModelBase* modelBase = ModelBase::GetInstance();
	// FString name = Lodash::GetInstanceId("LMLobby_");
	name = ModelBase::CheckGetName(name, "LMLobby");
	FVector rotation = FVector(0,0,0), location = FVector(0,0,0), scale = FVector(1,1,1);
	AActor* actor = modelBase->CreateActorEmpty(name, modelParams);
	modelParams.parent = actor->FindComponentByClass<USceneComponent>();

	FActorSpawnParameters spawnParams;
	FModelCreateParams createParams;
	createParams.parent = modelParams.parent;
	createParams.parentActor = actor;

	FString uName, type, pairsString, scaleString;
	TArray<FVector> vertices;
	TMap<FString, FPolygon> polygons = {};
	uName = name + "_Floor";
	pairsString = "meshRule=roomCube&mat=white&bottomMat=marbleTile&scale=" + DataConvert::VectorToString(size) +
		ModelBase::AddRotationString(createParamsIn.rotation);
	vertices = { createParamsIn.offset };
	polygons.Add(uName, FPolygon(uName, uName, vertices, FVector(0,0,0), "room", "point", pairsString));

	// Plants near walls
	float plantsWidth = 0.6;
	float planterBoxHeight = 0.3;
	float offsetFromWall = 1;
	float lengthBuffer = 2 * 2;
	FString plantOffsetAverage = "0.2";
	// FString bushesPairsString = "meshTypes=bush&placeOffsetAverage=" + plantOffsetAverage;
	FString bushesPairsString = "meshes=fern,solidFern,cinnamonFern&placeOffsetAverage=" + plantOffsetAverage;
	// Left wall
	scale = FVector(size.X - lengthBuffer, plantsWidth, planterBoxHeight);
	location = FVector(0, size.Y / -2 + offsetFromWall, 0);
	uName = Lodash::GetInstanceId("PlanterBox");
	pairsString = "meshRule=planterBox&scale=" + DataConvert::VectorToString(scale) +
		ModelBase::AddRotationString(createParamsIn.rotation);
	vertices = { MathVector::RotateVector(location, createParamsIn.rotation) + createParamsIn.offset };
	polygons.Add(uName, FPolygon(uName, uName, vertices, FVector(0,0,0), "planterBox", "point", pairsString));
	// plants
	location += FVector(0,0,planterBoxHeight);
	vertices = MathVector::RotateVertices(MathPolygon::PointToBox(location, scale), createParamsIn.rotation,
		createParamsIn.offset);
	uName = Lodash::GetInstanceId("Bush");
	pairsString = bushesPairsString;
	polygons.Add(uName, FPolygon(uName, uName, vertices, FVector(0,0,0), "bush", "polygon", pairsString));

	// Right wall
	scale = FVector(size.X - lengthBuffer, plantsWidth, planterBoxHeight);
	location = FVector(0, size.Y / 2 - offsetFromWall, 0);
	uName = Lodash::GetInstanceId("PlanterBox");
	pairsString = "meshRule=planterBox&scale=" + DataConvert::VectorToString(scale) +
		ModelBase::AddRotationString(createParamsIn.rotation);
	vertices = { MathVector::RotateVector(location, createParamsIn.rotation) + createParamsIn.offset };
	polygons.Add(uName, FPolygon(uName, uName, vertices, FVector(0,0,0), "planterBox", "point", pairsString));
	// plants
	location += FVector(0,0,planterBoxHeight);
	vertices = MathVector::RotateVertices(MathPolygon::PointToBox(location, scale), createParamsIn.rotation,
		createParamsIn.offset);
	uName = Lodash::GetInstanceId("Bush");
	pairsString = bushesPairsString;
	polygons.Add(uName, FPolygon(uName, uName, vertices, FVector(0,0,0), "bush", "polygon", pairsString));

	// Back wall
	scale = FVector(plantsWidth, size.Y - lengthBuffer, planterBoxHeight);
	location = FVector(size.X / -2 + offsetFromWall, 0, 0);
	uName = Lodash::GetInstanceId("PlanterBox");
	pairsString = "meshRule=planterBox&scale=" + DataConvert::VectorToString(scale) +
		ModelBase::AddRotationString(createParamsIn.rotation);
	vertices = { MathVector::RotateVector(location, createParamsIn.rotation) + createParamsIn.offset };
	polygons.Add(uName, FPolygon(uName, uName, vertices, FVector(0,0,0), "planterBox", "point", pairsString));
	// plants
	location += FVector(0,0,planterBoxHeight);
	vertices = MathVector::RotateVertices(MathPolygon::PointToBox(location, scale), createParamsIn.rotation,
		createParamsIn.offset);
	uName = Lodash::GetInstanceId("Bush");
	pairsString = bushesPairsString;
	polygons.Add(uName, FPolygon(uName, uName, vertices, FVector(0,0,0), "bush", "polygon", pairsString));

	// Front wall
	scale = FVector(plantsWidth, size.Y - lengthBuffer, planterBoxHeight);
	location = FVector(size.X / 2 - offsetFromWall, 0, 0);
	uName = Lodash::GetInstanceId("PlanterBox");
	pairsString = "meshRule=planterBox&scale=" + DataConvert::VectorToString(scale) +
		ModelBase::AddRotationString(createParamsIn.rotation);
	vertices = { MathVector::RotateVector(location, createParamsIn.rotation) + createParamsIn.offset };
	polygons.Add(uName, FPolygon(uName, uName, vertices, FVector(0,0,0), "planterBox", "point", pairsString));
	// plants
	location += FVector(0,0,planterBoxHeight);
	vertices = MathVector::RotateVertices(MathPolygon::PointToBox(location, scale), createParamsIn.rotation,
		createParamsIn.offset);
	uName = Lodash::GetInstanceId("Bush");
	pairsString = bushesPairsString;
	polygons.Add(uName, FPolygon(uName, uName, vertices, FVector(0,0,0), "bush", "polygon", pairsString));

	// modelParams.materialKey = "white";
	// TMap<FString, FString> materialKeys = {
	// 	{ "bottom", "marbleTile" }
	// };
	// BuildingRoom::Cube(size, modelParams, createParams, materialKeys);

	// modelParams.meshKey = "cube";
	// modelParams.materialKey = "marbleTile";
	// scale = FVector(size.X, size.Y, 0.2);
	// modelBase->CreateActor(name + "_Floor", FVector(0,0, -1 * scale.Z), rotation, scale, spawnParams, modelParams);

	// Ceiling
	// modelParams.meshKey = "cube";
	// modelParams.materialKey = "white";
	// scale = FVector(size.X, size.Y, 0.2);
	// modelBase->CreateActor(name + "_Ceiling", FVector(0,0, size.Z - scale.Z), rotation, scale, spawnParams, modelParams);

	// Lights

	scale = FVector(size.X / 2, size.Y / 2, -1);
	LMLobby::CouchesCoffeeTables(scale, modelParams, createParamsIn);

	// ModelBase::SetTransformFromParams(actor, createParamsIn);
	// return actor;
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
	polygons.Add(uName, FPolygon(uName, uName, vertices, FVector(0,0,0), "couch", "point", pairsString));

	uName = Lodash::GetInstanceId("Couch");
	pairsString = "mesh=couch3Cushions&dMat=leather&dMatColor=beige" +
		ModelBase::AddRotationString(createParamsIn.rotation);
	vertices = { MathVector::RotateVector(FVector(-3,0,0), createParamsIn.rotation) + createParamsIn.offset };
	polygons.Add(uName, FPolygon(uName, uName, vertices, FVector(0,0,0), "couch", "point", pairsString));

	uName = Lodash::GetInstanceId("Table");
	pairsString = "meshRule=tableRoundShort&scale=1.3,1.3,0.7" +
		ModelBase::AddRotationString(createParamsIn.rotation);
	vertices = { MathVector::RotateVector(FVector(0,0,0), createParamsIn.rotation) + createParamsIn.offset };
	polygons.Add(uName, FPolygon(uName, uName, vertices, FVector(0,0,0), "table", "point", pairsString));
	uName = Lodash::GetInstanceId("Table");
	pairsString = "meshRule=tableRoundShort&scale=0.6,0.6,0.7" +
		ModelBase::AddRotationString(createParamsIn.rotation);
	vertices = { MathVector::RotateVector(FVector(-1,-1,0), createParamsIn.rotation) + createParamsIn.offset };
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
	polygons.Add(uName, FPolygon(uName, uName, vertices, FVector(0,0,0), "chair", "point", pairsString));
	uName = Lodash::GetInstanceId("Chair");
	pairsString = "mesh=chair" + 
		ModelBase::AddRotationString(createParamsIn.rotation, FVector(0,0,-145));
	vertices = { MathVector::RotateVector(FVector(1.5,1.5,0), createParamsIn.rotation) + createParamsIn.offset };
	polygons.Add(uName, FPolygon(uName, uName, vertices, FVector(0,0,0), "chair", "point", pairsString));

	VerticesEdit* verticesEdit = VerticesEdit::GetInstance();
	verticesEdit->AddAndSave(polygons);
	return polygons;

	// ModelBase* modelBase = ModelBase::GetInstance();
	// FString name = Lodash::GetInstanceId("CouchesCoffeeTables_");
	// FVector rotation = FVector(0,0,0), location = FVector(0,0,0), scale = FVector(1,1,1);
	// AActor* actor = modelBase->CreateActorEmpty(name, modelParams);
	// modelParams.parent = actor->FindComponentByClass<USceneComponent>();

	// FModelCreateParams createParams;
	// createParams.parent = modelParams.parent;
	// createParams.parentActor = actor;
	// FActorSpawnParameters spawnParams;

	// FString modelKey;
	// LoadContent* loadContent = LoadContent::GetInstance();
	// modelKey = "couch2Cushions"
	// DynamicMaterial* dynamicMaterial = DynamicMaterial::GetInstance();
	// FString texturePathBase = loadContent->Texture("leather_base");
	// FString texturePathNormal = loadContent->Texture("leather_normal");
	// modelParams.dynamicMaterial = dynamicMaterial->CreateTextureColor(name + "_leather", texturePathBase,
	// 	texturePathNormal, DynamicMaterial::GetColor("beige"));
	// modelParams.meshKey = "couch2Cushions";
	// modelBase->CreateActor(name + "_Couch2Cushions", FVector(0, -3, 0), FVector(0,0,90), scale, spawnParams, modelParams);

	// modelParams.meshKey = "couch3Cushions";
	// modelBase->CreateActor(name + "_Couch3Cushions", FVector(-3, 0, 0), FVector(0,0,0), scale, spawnParams, modelParams);
	// modelParams.dynamicMaterial = "";

	// FModelParams modelParamsTable;
	// modelParamsTable.parent = modelParams.parent;
	// scale = FVector(1.3,1.3,0.7);
	// createParams.offset = FVector(0,0,0);
	// ModelTable::RoundShort(scale, modelParamsTable, createParams, {});

	// scale = FVector(0.6,0.6,0.7);
	// createParams.offset = FVector(-1,-1,0);
	// ModelTable::RoundShort(scale, modelParamsTable, createParams, {});
	// createParams.offset = FVector(-0.75,1.5,0);
	// ModelTable::RoundShort(scale, modelParamsTable, createParams, {});

	// modelParams.meshKey = "chair";
	// scale = FVector(1,1,1);
	// modelBase->CreateActor(name + "_Chair1", FVector(0.5, 2.5, 0), FVector(0,0,-115), scale, spawnParams, modelParams);
	// modelBase->CreateActor(name + "_Chair2", FVector(1.5, 1.5, 0), FVector(0,0,-145), scale, spawnParams, modelParams);

	// ModelBase::SetTransformFromParams(actor, createParamsIn);
	// return actor;
}
