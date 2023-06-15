#include "ModelLeg.h"

#include "Engine/StaticMeshActor.h"

#include "../ModelBase.h"
#include "../../Mesh/LoadContent.h"
#include "../../ModelingStructsActor.h"
#include "../../ProceduralModel/PMCylinder.h"

ModelLeg::ModelLeg() {
}

ModelLeg::~ModelLeg() {
}

void ModelLeg::FrontRight(FString name, FVector size, FVector scale, float buffer,
	USceneComponent* parent, FString mesh, FString material, FVector offset) {
	ModelBase* modelBase = ModelBase::GetInstance();
	FVector location = FVector(size.X / 2 - buffer, size.Y / 2 - buffer, 0) + offset;
	modelBase->CreateActor(name + "_LegFrontRight", location, FRotator(0,0,0), scale,
		FActorSpawnParameters(), parent, mesh, material);
}

void ModelLeg::BackRight(FString name, FVector size, FVector scale, float buffer,
	USceneComponent* parent, FString mesh, FString material, FVector offset) {
	ModelBase* modelBase = ModelBase::GetInstance();
	FVector location = FVector(-1 * size.X / 2 + buffer, size.Y / 2 - buffer, 0) + offset;
	modelBase->CreateActor(name + "_LegBackRight", location, FRotator(0,0,0), scale,
		FActorSpawnParameters(), parent, mesh, material);
}

void ModelLeg::FrontLeft(FString name, FVector size, FVector scale, float buffer,
	USceneComponent* parent, FString mesh, FString material, FVector offset) {
	ModelBase* modelBase = ModelBase::GetInstance();
	FVector location = FVector(size.X / 2 - buffer, -1 * size.Y / 2 + buffer, 0) + offset;
	modelBase->CreateActor(name + "_LegFrontLeft", location, FRotator(0,0,0), scale,
		FActorSpawnParameters(), parent, mesh, material);
}

void ModelLeg::BackLeft(FString name, FVector size, FVector scale, float buffer,
	USceneComponent* parent, FString mesh, FString material, FVector offset) {
	ModelBase* modelBase = ModelBase::GetInstance();
	FVector location = FVector(-1 * size.X / 2 + buffer, -1 * size.Y / 2 + buffer, 0) + offset;
	modelBase->CreateActor(name + "_LegBackLeft", location, FRotator(0,0,0), scale,
		FActorSpawnParameters(), parent, mesh, material);
}
