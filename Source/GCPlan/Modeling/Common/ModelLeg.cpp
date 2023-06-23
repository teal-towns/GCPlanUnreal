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
	FModelParams modelParams, FVector offset) {
	ModelBase* modelBase = ModelBase::GetInstance();
	FVector location = FVector(size.X / 2 - scale.X / 2 - buffer, size.Y / 2 - scale.Y / 2 - buffer, 0) + offset;
	modelBase->CreateActor(name + "_LegFrontRight", location, FVector(0,0,0), scale,
		FActorSpawnParameters(), modelParams);
}

void ModelLeg::BackRight(FString name, FVector size, FVector scale, float buffer,
	FModelParams modelParams, FVector offset) {
	ModelBase* modelBase = ModelBase::GetInstance();
	FVector location = FVector(-1 * size.X / 2 + scale.X / 2 + buffer, size.Y / 2 - scale.Y / 2 - buffer, 0) + offset;
	modelBase->CreateActor(name + "_LegBackRight", location, FVector(0,0,0), scale,
		FActorSpawnParameters(), modelParams);
}

void ModelLeg::FrontLeft(FString name, FVector size, FVector scale, float buffer,
	FModelParams modelParams, FVector offset) {
	ModelBase* modelBase = ModelBase::GetInstance();
	FVector location = FVector(size.X / 2 - scale.X / 2 - buffer, -1 * size.Y / 2 + scale.Y / 2 + buffer, 0) + offset;
	modelBase->CreateActor(name + "_LegFrontLeft", location, FVector(0,0,0), scale,
		FActorSpawnParameters(), modelParams);
}

void ModelLeg::BackLeft(FString name, FVector size, FVector scale, float buffer,
	FModelParams modelParams, FVector offset) {
	ModelBase* modelBase = ModelBase::GetInstance();
	FVector location = FVector(-1 * size.X / 2 + scale.X / 2 + buffer, -1 * size.Y / 2 + scale.Y / 2 + buffer, 0) + offset;
	modelBase->CreateActor(name + "_LegBackLeft", location, FVector(0,0,0), scale,
		FActorSpawnParameters(), modelParams);
}

void ModelLeg::Right(FString name, FVector size, FVector scale, float buffer,
	FModelParams modelParams, FVector offset) {
	ModelBase* modelBase = ModelBase::GetInstance();
	FVector location = FVector(0, size.Y / 2 - scale.Y / 2 - buffer, 0) + offset;
	modelBase->CreateActor(name + "_LegRight", location, FVector(0,0,0), scale,
		FActorSpawnParameters(), modelParams);
}

void ModelLeg::Left(FString name, FVector size, FVector scale, float buffer,
	FModelParams modelParams, FVector offset) {
	ModelBase* modelBase = ModelBase::GetInstance();
	FVector location = FVector(0, -1 * size.Y / 2 + scale.Y / 2 + buffer, 0) + offset;
	modelBase->CreateActor(name + "_LegLeft", location, FVector(0,0,0), scale,
		FActorSpawnParameters(), modelParams);
}

void ModelLeg::Back(FString name, FVector size, FVector scale, float buffer,
	FModelParams modelParams, FVector offset) {
	ModelBase* modelBase = ModelBase::GetInstance();
	FVector location = FVector(-1 * size.X / 2 + scale.X / 2 + buffer, 0, 0) + offset;
	modelBase->CreateActor(name + "_LegBack", location, FVector(0,0,0), scale,
		FActorSpawnParameters(), modelParams);
}

void ModelLeg::Front(FString name, FVector size, FVector scale, float buffer,
	FModelParams modelParams, FVector offset) {
	ModelBase* modelBase = ModelBase::GetInstance();
	FVector location = FVector(size.X / 2 - scale.X / 2 - buffer, 0, 0) + offset;
	modelBase->CreateActor(name + "_LegFront", location, FVector(0,0,0), scale,
		FActorSpawnParameters(), modelParams);
}
