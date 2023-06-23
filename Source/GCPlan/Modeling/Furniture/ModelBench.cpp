#include "ModelBench.h"

#include "../ModelBase.h"
#include "../../Common/Lodash.h"
#include "../../ProceduralModel/PMCylinder.h"

ModelBench::ModelBench() {
}

ModelBench::~ModelBench() {
}

AActor* ModelBench::CreateFromInputs() {
	ModelBase* modelBase = ModelBase::GetInstance();
	auto [modelingBase, modelParams] = modelBase->GetInputs("Bench", FVector(1,3,1));
	FString name = modelingBase.name;
	FVector size = modelingBase.size;
	TMap<FString, FString> pairs = modelingBase.pairs;
	return Create(size, modelParams, FModelCreateParams(), pairs);
}

AActor* ModelBench::Create(FVector size, FModelParams modelParams,
	FModelCreateParams createParamsIn, TMap<FString, FString> pairs) {
	ModelBase* modelBase = ModelBase::GetInstance();
	FString name = Lodash::GetInstanceId("Bench_");
	FVector rotation = FVector(0,0,0), location = FVector(0,0,0), scale = FVector(1,1,1);
	AActor* actor = modelBase->CreateActorEmpty(name, modelParams);
	modelParams.parent = actor->FindComponentByClass<USceneComponent>();

	modelParams.meshKey = "cube";
	modelParams.materialKey = "wood";
	FActorSpawnParameters spawnParams;

	location = FVector(0, 0, size.Z);
	scale = FVector(size.X, size.Y, 0.2);
	modelBase->CreateActor(name + "_Seat", location, rotation, scale, spawnParams, modelParams);

	// Legs
	scale = FVector(0.1, 0.1, size.Z);
	float buffer = 0.1;
	if (pairs.Contains("cylinderLegs")) {
		modelParams.mesh = PMCylinder::Create(name + "_Leg", scale, true);
		scale = FVector(1,1,1);
	}
	location = FVector(-0.5 * size.X + buffer, -0.5 * size.Y + buffer, 0);
	modelBase->CreateActor(name + "_Leg1", location, rotation, scale, spawnParams, modelParams);
	location = FVector(-0.5 * size.X + buffer, 0.5 * size.Y - buffer, 0);
	modelBase->CreateActor(name + "_Leg2", location, rotation, scale, spawnParams, modelParams);
	location = FVector(0.5 * size.X - buffer, -0.5 * size.Y + buffer, 0);
	modelBase->CreateActor(name + "_Leg3", location, rotation, scale, spawnParams, modelParams);
	location = FVector(0.5 * size.X - buffer, 0.5 * size.Y - buffer, 0);
	modelBase->CreateActor(name + "_Leg4", location, rotation, scale, spawnParams, modelParams);

	ModelBase::SetTransformFromParams(actor, createParamsIn);
	return actor;
}
