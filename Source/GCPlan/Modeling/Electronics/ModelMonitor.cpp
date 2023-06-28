#include "ModelMonitor.h"

#include "../ModelBase.h"
#include "../../Common/Lodash.h"
#include "../Common/ModelTile.h"

ModelMonitor::ModelMonitor() {
}

ModelMonitor::~ModelMonitor() {
}

AActor* ModelMonitor::CreateFromInputs() {
	ModelBase* modelBase = ModelBase::GetInstance();
	UWorld* World = modelBase->GetWorld();
	auto [modelingBase, modelParams] = modelBase->GetInputs("Monitor", FVector(1, 0.05, 0.8));
	FString name = modelingBase.name;
	FVector size = modelingBase.size;
	return Create(size, modelParams, FModelCreateParams());
}

AActor* ModelMonitor::Create(FVector size, FModelParams modelParams,
	FModelCreateParams createParamsIn) {
	ModelBase* modelBase = ModelBase::GetInstance();
	FString name = Lodash::GetInstanceId("Monitor_");
	FVector rotation = FVector(0,0,0), location = FVector(0,0,0), scale = FVector(1,1,1);
	AActor* actor = modelBase->CreateActorEmpty(name, modelParams);
	modelParams.parent = actor->FindComponentByClass<USceneComponent>();

	modelParams.meshKey = "cube";
	modelParams.materialKey = "black";
	FActorSpawnParameters spawnParams;

	float stand = 0.1;
	// Front
	location = FVector(0, 0, stand);
	scale = FVector(size.X, size.Y, size.Z);
	modelBase->CreateActor(name + "_Front", location, rotation, scale, spawnParams, modelParams);

	if (stand > 0) {
		// Back
		float thick = 0.05;
		location = FVector((size.X / -2.0), 0, thick);
		scale = FVector(stand, thick, (2.0 * stand));
		modelBase->CreateActor(name + "_Back", location, rotation, scale, spawnParams, modelParams);

		// Base
		location = FVector((-0.5 * size.X), 0, thick);
		scale = FVector((3.0 * size.X), (3.0 * stand), thick);
		modelBase->CreateActor(name + "_Base", location, rotation, scale, spawnParams, modelParams);
	}

	ModelBase::SetTransformFromParams(actor, createParamsIn);
	return actor;
}
