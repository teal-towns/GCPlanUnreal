#include "ModelBench.h"

#include "Engine/StaticMeshActor.h"

#include "ModelBase.h"
#include "../Mesh/LoadContent.h"
#include "../ModelingStructsActor.h"
#include "../ProceduralModel/PMCylinder.h"

ModelBench::ModelBench() {
}

ModelBench::~ModelBench() {
}

void ModelBench::Create() {
	ModelBase* modelBase = ModelBase::GetInstance();
	UWorld* World = modelBase->GetWorld();
	FModelingBase modelingBase = modelBase->GetInputs("Bench1", FVector(3,1,1));
	FString name = modelingBase.name;
	FVector size = modelingBase.size;
	TArray<FString> tags = modelingBase.tags;

	FRotator rotation = FRotator(0,0,0);
	FActorSpawnParameters spawnParams;
	FVector location = FVector(0,0,0);
	FVector scale = FVector(1,1,1);
	AStaticMeshActor* actor;

	// Parent container
	actor = modelBase->CreateActor(name, location, rotation, scale, spawnParams);
	USceneComponent* parent = actor->FindComponentByClass<USceneComponent>();

	LoadContent* loadContent = LoadContent::GetInstance();
	FString meshPath = loadContent->Mesh("cube");
	FString materialPath = loadContent->Material("wood");
	FModelParams modelParams;
	modelParams.meshPath = meshPath;
	modelParams.materialPath = materialPath;
	modelParams.parent = parent;

	// Seat
	location = FVector(0, 0, size.Z);
	scale = FVector(size.X, size.Y, 0.2);
	modelBase->CreateActor(name + "_Seat", location, rotation, scale, spawnParams, modelParams);

	// Legs
	spawnParams.Owner = actor;
	scale = FVector(0.1, 0.1, size.Z);
	float buffer = 0.1;

	UStaticMesh* mesh = nullptr;
	if (tags.Contains("cylinderLegs")) {
		modelParams.mesh = PMCylinder::Create(name + "_Leg", scale, FVector(10,10,10), {}, true);
		// Already scaled, so reset.
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
}
