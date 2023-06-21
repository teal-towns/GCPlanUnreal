#include "ModelTable.h"

#include "../ModelBase.h"
#include "../Common/ModelLeg.h"
#include "../../Common/Lodash.h"

ModelTable::ModelTable() {
}

ModelTable::~ModelTable() {
}

void ModelTable::Build(TMap<FString, FString> pairs) {
	auto [key, modelParams] = ModelBase::ModelParamsFromPairs(pairs);
	auto [location, rotation, scale] = ModelBase::PairsToTransform(pairs, FVector(0,0,0));
	FModelCreateParams createParams;
	createParams.offset = location;
	createParams.rotation = rotation;
	if (!pairs.Contains("meshRule")) {
		pairs.Add("meshRule", "");
	}
	if (pairs["meshRule"] == "tableRoundShort") {
		RoundShort(scale, modelParams, createParams);
	}
}

AActor* ModelTable::CreateFromInputs() {
	ModelBase* modelBase = ModelBase::GetInstance();
	auto [modelingBase, modelParams] = modelBase->GetInputs("Table", FVector(1.5,2.5,1));
	FString name = modelingBase.name;
	FVector size = modelingBase.size;
	TArray<FString> tags = modelingBase.tags;
	if (tags.Contains("roundShort")) {
		return RoundShort(size, modelParams, FModelCreateParams());
	}
	return Create(size, modelParams, FModelCreateParams(), tags);
}

AActor* ModelTable::Create(FVector size, FModelParams modelParams,
	FModelCreateParams createParamsIn, TArray<FString> tags) {
	ModelBase* modelBase = ModelBase::GetInstance();
	FString name = Lodash::GetInstanceId("Table_");
	FVector rotation = FVector(0,0,0), location = FVector(0,0,0), scale = FVector(1,1,1);
	AActor* actor = modelBase->CreateActorEmpty(name, modelParams);
	modelParams.parent = actor->FindComponentByClass<USceneComponent>();

	modelParams.meshKey = "cube";
	modelParams.materialKey = "wood";
	FActorSpawnParameters spawnParams;

	// spawnParams.Owner = actor;
	float thick = 0.1;
	float legZ = size.Z - thick;
	float legsOffset = 0.2;

	if (tags.Contains("metal")) {
		modelParams.materialKey = "metalChrome";
		modelParams.meshKey = "cylinder";
		legsOffset = 0.4;
	}

	FVector scaleLeg = FVector(thick, thick, legZ);
	ModelLeg::FrontRight(name, size, scaleLeg, legsOffset, modelParams);
	ModelLeg::BackRight(name, size, scaleLeg, legsOffset, modelParams);
	ModelLeg::BackLeft(name, size, scaleLeg, legsOffset, modelParams);
	ModelLeg::FrontLeft(name, size, scaleLeg, legsOffset, modelParams);

	location = FVector(0, 0, legZ);
	scale = FVector(size.X, size.Y, thick);
	modelBase->CreateActor(name + "_Top", location, rotation, scale, spawnParams, modelParams);

	ModelBase::SetTransformFromParams(actor, createParamsIn);
	return actor;
}

AActor* ModelTable::RoundShort(FVector size, FModelParams modelParams,
	FModelCreateParams createParamsIn) {
	if (size == FVector(0,0,0)) {
		size = FVector(1.3,1.3,0.7);
	}
	FString name = Lodash::GetInstanceId("TableRoundShort_");
	ModelBase* modelBase = ModelBase::GetInstance();
	FVector scale = FVector(1,1,1), rotation = FVector(0,0,0), location = FVector(0,0,0);
	AActor* actor = modelBase->CreateActorEmpty(name, modelParams);
	modelParams.parent = actor->FindComponentByClass<USceneComponent>();

	FActorSpawnParameters spawnParams;
	float topHeight = 0.05;
	// Legs: 2 crossed pieces.
	modelParams.meshKey = "cube";
	modelParams.materialKey = "metalChrome";
	scale = FVector(size.X * 0.9, 0.05, size.Z - topHeight);
	modelBase->CreateActor(name + "_Leg1", location, rotation, scale, spawnParams, modelParams);
	scale = FVector(size.Y * 0.9, 0.05, size.Z - topHeight);
	modelBase->CreateActor(name + "_Leg2", location, FVector(0,0,90), scale, spawnParams, modelParams);
	// Top
	modelParams.meshKey = "cylinder";
	modelParams.materialKey = "white";
	scale = FVector(size.X, size.Y, topHeight);
	modelBase->CreateActor(name + "_Top", FVector(0,0,size.Z - topHeight), rotation, scale, spawnParams, modelParams);
	ModelBase::SetTransformFromParams(actor, createParamsIn);
	return actor;
}
