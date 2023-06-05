#include "ModelBench.h"

#include "Engine/StaticMeshActor.h"

#include "../ModelingStructsActor.h"
#include "ModelBase.h"

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
	AStaticMeshActor* bench = modelBase->CreateActor(name, location, rotation, scale, spawnParams);
	USceneComponent* parent = bench->FindComponentByClass<USceneComponent>();

	FString meshPath = "/Script/Engine.StaticMesh'/Game/Modeling/Primitives/Cube.Cube'";
	FString materialPath = "/Script/Engine.Material'/Game/Nature/Wood/wood-pale-material.wood-pale-material'";

	// Seat
	location = FVector(0, 0, size.Z);
	scale = FVector(size.X, size.Y, 0.2);
	modelBase->CreateActor(name + "_Leg4", location, rotation, scale, spawnParams, parent, meshPath, materialPath);

	// Legs
	spawnParams.Owner = bench;
	scale = FVector(0.1, 0.1, size.Z);
	float buffer = 0.1;

	location = FVector(-0.5 * size.X + buffer, -0.5 * size.Y + buffer, 0);
	modelBase->CreateActor(name + "_Leg1", location, rotation, scale, spawnParams, parent, meshPath, materialPath);

	location = FVector(-0.5 * size.X + buffer, 0.5 * size.Y - buffer, 0);
	modelBase->CreateActor(name + "_Leg2", location, rotation, scale, spawnParams, parent, meshPath, materialPath);

	location = FVector(0.5 * size.X - buffer, -0.5 * size.Y + buffer, 0);
	modelBase->CreateActor(name + "_Leg3", location, rotation, scale, spawnParams, parent, meshPath, materialPath);

	location = FVector(0.5 * size.X - buffer, 0.5 * size.Y - buffer, 0);
	modelBase->CreateActor(name + "_Leg4", location, rotation, scale, spawnParams, parent, meshPath, materialPath);
}
