#include "ModelDesk.h"

#include "Engine/StaticMeshActor.h"

#include "ModelBase.h"
#include "../ModelingStructsActor.h"
#include "../ProceduralModel/PMCylinder.h"

/////////////////////////////////////////////////////////////////////////////////////////////
ModelDesk::ModelDesk() {
}

ModelDesk::~ModelDesk() {
}

void ModelDesk::Create() {
	ModelBase* modelBase = ModelBase::GetInstance();
	UWorld* World = modelBase->GetWorld();
	FModelingBase modelingBase = modelBase->GetInputs("Desk1", FVector(4,2,3));
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

	FString meshPath = "/Script/Engine.StaticMesh'/Game/Modeling/Primitives/Cube.Cube'";
	FString materialPath = "/Script/Engine.Material'/Game/Nature/Wood/wood-pale-material.wood-pale-material'";

	spawnParams.Owner = actor;
	UStaticMesh* mesh = nullptr;
	float thick = 0.1;

	// Top
	location = FVector(0, 0, size.Z);
	scale = FVector(size.X, size.Y, thick);
	modelBase->CreateActor(name + "_Top", location, rotation, scale, spawnParams, parent, meshPath, materialPath);

	// Left
	location = FVector(((-0.5 * size.X) + (0.5 * thick)), 0, 0);
	scale = FVector(thick, size.Y, size.Z);
	modelBase->CreateActor(name + "_Left", location, rotation, scale, spawnParams, parent, meshPath, materialPath);

	// Right
	location = FVector(((0.5 * size.X) - (0.5 * thick)), 0, 0);
	scale = FVector(thick, size.Y, size.Z);
	modelBase->CreateActor(name + "_Right", location, rotation, scale, spawnParams, parent, meshPath, materialPath);

	// Back
	float gapBack = (0.2 * size.Z);
	location = FVector(0, ((-0.5 * size.Y) + (0.5 * thick)), gapBack);
	scale = FVector((size.X - (2.0 * thick)), thick, (size.Z - gapBack));
	modelBase->CreateActor(name + "_Back", location, rotation, scale, spawnParams, parent, meshPath, materialPath);

	if (tags.Contains("KVM")) {
		FVector defLocation = FVector(0, (-0.25 * size.Y), (size.Z + thick));
		ModelMonitor::Create(defLocation);
		defLocation = FVector((-0.10 * size.X), 0, (size.Z + thick));
		ModelKeyboard::Create(defLocation);
		defLocation = FVector((0.10 * size.X), 0, (size.Z + thick));
		ModelMouse::Create(defLocation);
	}

} // ModelDesk

/////////////////////////////////////////////////////////////////////////////////////////////
ModelMonitor::ModelMonitor() {
}

ModelMonitor::~ModelMonitor() {
}

void ModelMonitor::Create(FVector defLocation) {
	ModelBase* modelBase = ModelBase::GetInstance();
	UWorld* World = modelBase->GetWorld();
	FModelingBase modelingBase = modelBase->GetInputs("Monitor1", FVector(1, 0.05, 0.8));
	FString name = modelingBase.name;
	FVector size = modelingBase.size;
	TArray<FString> tags = modelingBase.tags;

	FRotator rotation = FRotator(0, 0, 0);
	FActorSpawnParameters spawnParams;
	FVector location = defLocation; // FVector(0, 0, 0);
	FVector scale = FVector(1, 1, 1);
	AStaticMeshActor* actor;

	// Parent container
	actor = modelBase->CreateActor(name, location, rotation, scale, spawnParams);
	USceneComponent* parent = actor->FindComponentByClass<USceneComponent>();

	FString meshPath = "/Script/Engine.StaticMesh'/Game/Modeling/Primitives/Cube.Cube'";
	FString materialPath = "/Script/Engine.Material'/Game/Landscape/VerticesEdit/Black_M.Black_M'";

	spawnParams.Owner = actor;
	UStaticMesh* mesh = nullptr;
	float stand = 0;// .05;

	// Front
	location = FVector(0, 0, stand);
	scale = FVector(size.X, size.Y, size.Z);
	modelBase->CreateActor(name + "_Front", location, rotation, scale, spawnParams, parent, meshPath, materialPath);

	if (stand > 0) {
		// Back
		float thick = 0.05;
		location = FVector((-0.5 * stand), ((-0.5 * size.Y) + (0.5 * thick)), (stand * 2));
		scale = FVector(stand, thick, (stand * 2));
		modelBase->CreateActor(name + "_Back", location, rotation, scale, spawnParams, parent, meshPath, materialPath);
	} // stand
} // ModelMonitor

/////////////////////////////////////////////////////////////////////////////////////////////
ModelKeyboard::ModelKeyboard() {
}

ModelKeyboard::~ModelKeyboard() {
}

void ModelKeyboard::Create(FVector defLocation) {
	ModelBase* modelBase = ModelBase::GetInstance();
	UWorld* World = modelBase->GetWorld();
	FModelingBase modelingBase = modelBase->GetInputs("Keyboard1", FVector(0.5, 0.25, 0.05));
	FString name = modelingBase.name;
	FVector size = modelingBase.size;
	TArray<FString> tags = modelingBase.tags;

	FRotator rotation = FRotator(0, 0, 0);
	FActorSpawnParameters spawnParams;
	FVector location = defLocation; // FVector(0, 0, 0);
	FVector scale = FVector(1, 1, 1);
	AStaticMeshActor* actor;

	// Parent container
	actor = modelBase->CreateActor(name, location, rotation, scale, spawnParams);
	USceneComponent* parent = actor->FindComponentByClass<USceneComponent>();

	FString meshPath = "/Script/Engine.StaticMesh'/Game/Modeling/Primitives/Cube.Cube'";
	FString materialPath = "/Script/Engine.Material'/Game/Landscape/VerticesEdit/Grey_M.Grey_M'";

	spawnParams.Owner = actor;
	UStaticMesh* mesh = nullptr;

	// Top
	location = FVector(0, 0, 0);
	scale = FVector(size.X, size.Y, size.Z);
	modelBase->CreateActor(name + "_Top", location, rotation, scale, spawnParams, parent, meshPath, materialPath);
} // ModelKeyboard

/////////////////////////////////////////////////////////////////////////////////////////////
ModelMouse::ModelMouse() {
}

ModelMouse::~ModelMouse() {
}

void ModelMouse::Create(FVector defLocation) {
	ModelBase* modelBase = ModelBase::GetInstance();
	UWorld* World = modelBase->GetWorld();
	FModelingBase modelingBase = modelBase->GetInputs("Mouse1", FVector(0.06, 0.12, 0.05));
	FString name = modelingBase.name;
	FVector size = modelingBase.size;
	TArray<FString> tags = modelingBase.tags;

	FRotator rotation = FRotator(0, 0, 0);
	FActorSpawnParameters spawnParams;
	FVector location = defLocation; // FVector(0, 0, 0);
	FVector scale = FVector(1, 1, 1);
	AStaticMeshActor* actor;

	// Parent container
	actor = modelBase->CreateActor(name, location, rotation, scale, spawnParams);
	USceneComponent* parent = actor->FindComponentByClass<USceneComponent>();

	FString meshPath = "/Script/Engine.StaticMesh'/Game/Modeling/Primitives/Cube.Cube'";
	FString materialPath = "/Script/Engine.Material'/Game/Landscape/VerticesEdit/Blue_M.Blue_M'";

	spawnParams.Owner = actor;
	UStaticMesh* mesh = nullptr;

	// Top
	location = FVector(0, 0, 0);
	scale = FVector(size.X, size.Y, size.Z);
	modelBase->CreateActor(name + "_Top", location, rotation, scale, spawnParams, parent, meshPath, materialPath);
} // ModelMouse