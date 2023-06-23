#include "ModelComputer.h"

#include "../ModelBase.h"
#include "../../Common/Lodash.h"

ModelComputer::ModelComputer() {
}

ModelComputer::~ModelComputer() {
}

AActor* ModelComputer::Keyboard(FVector size, FModelParams modelParams,
	FModelCreateParams createParamsIn) {

	ModelBase* modelBase = ModelBase::GetInstance();
	FString name = Lodash::GetInstanceId("Keyboard_");
	FVector rotation = FVector(0,0,0), location = FVector(0,0,0), scale = FVector(1,1,1);
	AActor* actor = modelBase->CreateActorEmpty(name, modelParams);
	modelParams.parent = actor->FindComponentByClass<USceneComponent>();

	modelParams.meshKey = "cube";
	modelParams.materialKey = "grey";
	FActorSpawnParameters spawnParams;

	location = FVector(0, 0, 0);
	scale = FVector(size.X, size.Y, size.Z);
	modelBase->CreateActor(name + "_Top", location, rotation, scale, spawnParams, modelParams);

	ModelBase::SetTransformFromParams(actor, createParamsIn);
	return actor;
}

AActor* ModelComputer::Mouse(FVector size, FModelParams modelParams,
	FModelCreateParams createParamsIn) {

	ModelBase* modelBase = ModelBase::GetInstance();
	FString name = Lodash::GetInstanceId("Mouse_");
	FVector rotation = FVector(0,0,0), location = FVector(0,0,0), scale = FVector(1,1,1);
	AActor* actor = modelBase->CreateActorEmpty(name, modelParams);
	modelParams.parent = actor->FindComponentByClass<USceneComponent>();

	modelParams.meshKey = "cube";
	modelParams.materialKey = "blue";
	FActorSpawnParameters spawnParams;

	location = FVector(0, 0, 0);
	scale = FVector(size.X, size.Y, size.Z);
	modelBase->CreateActor(name + "_Top", location, rotation, scale, spawnParams, modelParams);

	ModelBase::SetTransformFromParams(actor, createParamsIn);
	return actor;
}
