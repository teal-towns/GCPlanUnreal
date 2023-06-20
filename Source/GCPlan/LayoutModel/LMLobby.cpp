#include "LMLobby.h"

#include "Engine/StaticMeshActor.h"

#include "LayoutModelBase.h"
#include "../Common/Lodash.h"
#include "../Modeling/ModelBase.h"
#include "../Modeling/Furniture/ModelTable.h"
#include "../Mesh/DynamicMaterial.h"
#include "../Mesh/LoadContent.h"
#include "../ModelingStructsActor.h"

LMLobby::LMLobby() {
}

LMLobby::~LMLobby() {
}

AActor* LMLobby::CreateFromInputs() {
	ModelBase* modelBase = ModelBase::GetInstance();
	LayoutModelBase* layoutModelBase = LayoutModelBase::GetInstance();
	UWorld* World = layoutModelBase->GetWorld();
	auto [layoutParams, modelParams] = layoutModelBase->GetInputs("Lobby", FVector(10,12,4));
	FString name = layoutParams.name;
	FVector size = layoutParams.size;
	TArray<FString> tags = layoutParams.tags;
	return Create(size, modelParams, FModelCreateParams(), name);
}

AActor* LMLobby::Create(FVector size, FModelParams modelParams,
	FModelCreateParams createParamsIn, FString name) {
	ModelBase* modelBase = ModelBase::GetInstance();
	name = ModelBase::CheckGetName(name, "LMLobby");
	FVector rotation = FVector(0,0,0);
	FActorSpawnParameters spawnParams;
	FVector location = FVector(0,0,0);
	FVector scale = FVector(1,1,1);
	AStaticMeshActor* actor;

	actor = modelBase->CreateActor(name, location, rotation, scale, spawnParams);
	USceneComponent* parent = actor->FindComponentByClass<USceneComponent>();
	modelParams.parent = parent;

	modelParams.meshKey = "cube";
	modelParams.materialKey = "marbleTile";
	scale = FVector(size.X, size.Y, 0.2);
	modelBase->CreateActor(name + "_Floor", FVector(0,0, -1 * scale.Z), rotation, scale, spawnParams, modelParams);

	// Ceiling with lights
	// TODO

	scale = FVector(size.X / 2, size.Y / 2, -1);
	LMLobby::CouchesCoffeeTables(scale, modelParams, createParamsIn);

	// Stand with books

	// Plants near walls
	// TODO

	ModelBase::SetTransformFromParams(actor, createParamsIn);
	return actor;
}

AActor* LMLobby::CouchesCoffeeTables(FVector size, FModelParams modelParams,
	FModelCreateParams createParamsIn) {
	FString name = Lodash::GetInstanceId("CouchesCoffeeTables");
	ModelBase* modelBase = ModelBase::GetInstance();
	FVector location = FVector(0,0,0), rotation = FVector(0,0,0), scale = FVector(1,1,1);
	FActorSpawnParameters spawnParams;
	FModelCreateParams createParams;
	AActor* actor = modelBase->CreateActorEmpty(name, modelParams);
	createParams.parent = actor->FindComponentByClass<USceneComponent>();
	createParams.parentActor = actor;
	modelParams.parent = createParams.parent;

	LoadContent* loadContent = LoadContent::GetInstance();
	DynamicMaterial* dynamicMaterial = DynamicMaterial::GetInstance();
	FString texturePathBase = loadContent->Texture("leather_base");
	FString texturePathNormal = loadContent->Texture("leather_normal");
	modelParams.dynamicMaterial = dynamicMaterial->CreateTextureColor(name + "_leather", texturePathBase,
		texturePathNormal, DynamicMaterial::GetColor("beige"));
	modelParams.meshKey = "couch2Cushions";
	modelBase->CreateActor(name + "_Couch2Cushions", FVector(0, -3, 0), FVector(0,0,90), scale, spawnParams, modelParams);

	modelParams.meshKey = "couch3Cushions";
	modelBase->CreateActor(name + "_Couch3Cushions", FVector(-3, 0, 0), FVector(0,0,0), scale, spawnParams, modelParams);

	FModelParams modelParamsTable;
	modelParamsTable.parent = modelParams.parent;
	scale = FVector(1,1,0.7);
	createParams.offset = FVector(0,0,0);
	ModelTable::RoundShort(scale, modelParamsTable, createParams, {});

	scale = FVector(0.4,0.4,0.7);
	createParams.offset = FVector(-1,-1,0);
	ModelTable::RoundShort(scale, modelParamsTable, createParams, {});
	createParams.offset = FVector(-0.5,2,0);
	ModelTable::RoundShort(scale, modelParamsTable, createParams, {});

	// TODO - chair(s)

	ModelBase::SetTransformFromParams(actor, createParamsIn);
	return actor;
}
