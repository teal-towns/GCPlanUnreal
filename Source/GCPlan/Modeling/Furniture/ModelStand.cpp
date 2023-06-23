#include "ModelStand.h"

#include "../ModelBase.h"
#include "../Common/ModelSide.h"
#include "../../Common/Lodash.h"
#include "../../ModelingStructsActor.h"

ModelStand::ModelStand() {
}

ModelStand::~ModelStand() {
}

AActor* ModelStand::CreateFromInputs() {
	ModelBase* modelBase = ModelBase::GetInstance();
	auto [modelingBase, modelParams] = modelBase->GetInputs("Stand", FVector(3,2,1));
	FString name = modelingBase.name;
	FVector size = modelingBase.size;
	return Create(size, modelParams, FModelCreateParams());
}

AActor* ModelStand::Create(FVector size, FModelParams modelParams,
	FModelCreateParams createParamsIn) {
	ModelBase* modelBase = ModelBase::GetInstance();
	FString name = Lodash::GetInstanceId("Stand_");
	FVector rotation = FVector(0,0,0), location = FVector(0,0,0), scale = FVector(1,1,1);
	AActor* actor = modelBase->CreateActorEmpty(name, modelParams);
	modelParams.parent = actor->FindComponentByClass<USceneComponent>();

	modelParams.meshKey = "cube";
	modelParams.materialKey = "metalChrome";
	scale = FVector(0.1,0.1,0.1);
	ModelSide::Right(name, size, scale, modelParams);
	ModelSide::Left(name, size, scale, modelParams);
	ModelSide::Top(name, size, scale, modelParams);

	ModelBase::SetTransformFromParams(actor, createParamsIn);
	return actor;
}
