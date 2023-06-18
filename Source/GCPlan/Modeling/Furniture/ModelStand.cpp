#include "ModelStand.h"

#include "../ModelBase.h"
#include "../Common/ModelSide.h"
#include "../../ModelingStructsActor.h"

ModelStand::ModelStand() {
}

ModelStand::~ModelStand() {
}

AActor* ModelStand::Create() {
	ModelBase* modelBase = ModelBase::GetInstance();
	auto [modelingBase, modelParams] = modelBase->GetInputs("Stand", FVector(3,2,1));
	FString name = modelingBase.name;
	FVector size = modelingBase.size;
	TArray<FString> tags = modelingBase.tags;

	AActor* actor = modelBase->CreateActor(name);
	modelParams.parent = actor->FindComponentByClass<USceneComponent>();

	FVector scale = FVector(1,1,1), rotation = FVector(0,0,0), location = FVector(0,0,0);
	FActorSpawnParameters spawnParams;

	modelParams.meshKey = "cube";
	modelParams.materialKey = "metalChrome";
	scale = FVector(0.1,0.1,0.1);
	ModelSide::Right(name, size, scale, modelParams);
	ModelSide::Left(name, size, scale, modelParams);
	ModelSide::Top(name, size, scale, modelParams);

	return actor;
}
