#include "ModelSide.h"

#include "Engine/StaticMeshActor.h"

#include "../ModelBase.h"
#include "../../Mesh/LoadContent.h"
#include "../../ModelingStructsActor.h"
#include "../../ProceduralModel/PMCylinder.h"

ModelSide::ModelSide() {
}

ModelSide::~ModelSide() {
}

void ModelSide::Left(FString name, FVector size, FVector scale, FModelParams modelParams,
	FVector offset) {
	ModelBase* modelBase = ModelBase::GetInstance();
	scale = FVector(size.X, scale.Y, size.Z);
	FVector location = FVector(0, -1 * size.Y / 2 + scale.Y / 2, 0) + offset;
	modelBase->CreateActor(name + "_Left", location, FVector(0,0,0), scale,
		FActorSpawnParameters(), modelParams);
}

void ModelSide::Right(FString name, FVector size, FVector scale, FModelParams modelParams,
	FVector offset) {
	ModelBase* modelBase = ModelBase::GetInstance();
	scale = FVector(size.X, scale.Y, size.Z);
	FVector location = FVector(0, size.Y / 2 - scale.Y / 2, 0) + offset;
	modelBase->CreateActor(name + "_Right", location, FVector(0,0,0), scale,
		FActorSpawnParameters(), modelParams);
}

void ModelSide::Back(FString name, FVector size, FVector scale, FModelParams modelParams,
	FVector offset) {
	ModelBase* modelBase = ModelBase::GetInstance();
	scale = FVector(scale.X, size.Y, size.Z);
	FVector location = FVector(-1 * size.X / 2 + scale.X / 2, 0, 0) + offset;
	modelBase->CreateActor(name + "_Back", location, FVector(0,0,0), scale,
		FActorSpawnParameters(), modelParams);
}

void ModelSide::Front(FString name, FVector size, FVector scale, FModelParams modelParams,
	FVector offset) {
	ModelBase* modelBase = ModelBase::GetInstance();
	scale = FVector(scale.X, size.Y, size.Z);
	FVector location = FVector(size.X / 2 - scale.X / 2, 0, 0) + offset;
	modelBase->CreateActor(name + "_Front", location, FVector(0,0,0), scale,
		FActorSpawnParameters(), modelParams);
}

void ModelSide::Bottom(FString name, FVector size, FVector scale, FModelParams modelParams,
	FVector offset) {
	ModelBase* modelBase = ModelBase::GetInstance();
	scale = FVector(size.X, size.Y, scale.Z);
	FVector location = FVector(0, 0, 0) + offset;
	modelBase->CreateActor(name + "_Bottom", location, FVector(0,0,0), scale,
		FActorSpawnParameters(), modelParams);
}

void ModelSide::Top(FString name, FVector size, FVector scale, FModelParams modelParams,
	FVector offset) {
	ModelBase* modelBase = ModelBase::GetInstance();
	scale = FVector(size.X, size.Y, scale.Z);
	FVector location = FVector(0, 0, size.Z - scale.Z) + offset;
	modelBase->CreateActor(name + "_Top", location, FVector(0,0,0), scale,
		FActorSpawnParameters(), modelParams);
}
