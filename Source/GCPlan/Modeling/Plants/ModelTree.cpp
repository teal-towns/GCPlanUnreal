#include "ModelTree.h"

#include "../ModelBase.h"
#include "../../Common/Lodash.h"

ModelTree::ModelTree() {
}

ModelTree::~ModelTree() {
}

AActor* ModelTree::CreateFromInputs() {
	ModelBase* modelBase = ModelBase::GetInstance();
	UWorld* World = modelBase->GetWorld();
	auto [modelingBase, modelParams] = modelBase->GetInputs("Tree", FVector(1, 1, 20));
	FString name = modelingBase.name;
	FVector size = modelingBase.size;
	return Create(size, modelParams, FModelCreateParams());
}

AActor* ModelTree::Create(FVector size, FModelParams modelParams,
	FModelCreateParams createParamsIn) {
	ModelBase* modelBase = ModelBase::GetInstance();
	FString name = Lodash::GetInstanceId("Tree_");
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
