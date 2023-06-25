#include "ModelCord.h"

#include "Engine/StaticMeshActor.h"

#include "../ModelBase.h"
#include "../../Common/Lodash.h"
#include "../../Mesh/LoadContent.h"
#include "../../ModelingStructsActor.h"
#include "../../ProceduralModel/PMCylinder.h"

ModelCord::ModelCord() {
}

ModelCord::~ModelCord() {
}

void ModelCord::Build(TMap<FString, FString> pairs) {
	auto [key, modelParams] = ModelBase::ModelParamsFromPairs(pairs);
	auto [location, rotation, scale] = ModelBase::PairsToTransform(pairs, FVector(0,0,0));
	FModelCreateParams createParams;
	createParams.offset = location;
	createParams.rotation = rotation;
	if (!pairs.Contains("meshRule")) {
		pairs.Add("meshRule", "");
	}
	Create(scale, modelParams, createParams, pairs);
}

AActor* ModelCord::Create(FVector size, FModelParams modelParams,
	FModelCreateParams createParamsIn, TMap<FString, FString> pairs) {
	ModelBase* modelBase = ModelBase::GetInstance();
	FString name = Lodash::GetInstanceId("Cord_");
	FVector rotation = FVector(0,0,0), location = FVector(0,0,0), scale = FVector(1,1,1);
	AActor* actor = modelBase->CreateActorEmpty(name, modelParams);
	modelParams.parent = actor->FindComponentByClass<USceneComponent>();

	modelParams.meshKey = "cube";
	modelParams.materialKey = "black";
	FActorSpawnParameters spawnParams;

	scale = FVector(size.X, size.Y, size.Z);
	modelBase->CreateActor(name + "_cord", location, rotation, scale,
		FActorSpawnParameters(), modelParams);

	ModelBase::SetTransformFromParams(actor, createParamsIn);
	return actor;
}
