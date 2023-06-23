#include "ModelBush.h"

#include "../ModelBase.h"
#include "../../Common/Lodash.h"

ModelBush::ModelBush() {
}

ModelBush::~ModelBush() {
}

AActor* ModelBush::CreateFromInputs() {
	ModelBase* modelBase = ModelBase::GetInstance();
	UWorld* World = modelBase->GetWorld();
	auto [modelingBase, modelParams] = modelBase->GetInputs("Bush", FVector(1.5, 1.5, 2));
	FString name = modelingBase.name;
	FVector size = modelingBase.size;
	return Create(size, modelParams, FModelCreateParams());
}

AActor* ModelBush::Create(FVector size, FModelParams modelParams,
	FModelCreateParams createParamsIn) {
	ModelBase* modelBase = ModelBase::GetInstance();
	FString name = Lodash::GetInstanceId("Bush_");
	FVector rotation = FVector(0,0,0), location = FVector(0,0,0), scale = FVector(1,1,1);
	AActor* actor = modelBase->CreateActorEmpty(name, modelParams);
	modelParams.parent = actor->FindComponentByClass<USceneComponent>();

	// TODO
	FActorSpawnParameters spawnParams;
	modelParams.meshKey = "cylinder";
	modelParams.materialKey = "wood";
	modelBase->CreateActor(name + "_Trunk", location, rotation, size, spawnParams, modelParams);

	ModelBase::SetTransformFromParams(actor, createParamsIn);
	return actor;
}
