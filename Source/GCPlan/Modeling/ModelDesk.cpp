#include "ModelDesk.h"

#include "Engine/StaticMeshActor.h"

#include "ModelBase.h"
#include "../Mesh/LoadContent.h"
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
	auto [modelingBase, modelParams] = modelBase->GetInputs("Desk1", FVector(4,2,3));
	FString name = modelingBase.name;
	FVector size = modelingBase.size;
	TArray<FString> tags = modelingBase.tags;

	FVector rotation = FVector(0,0,0);
	FActorSpawnParameters spawnParams;
	FVector location = FVector(0,0,0);
	FVector scale = FVector(1,1,1);
	AStaticMeshActor* actor;

	// Parent container
	actor = modelBase->CreateActor(name, location, rotation, scale, spawnParams);
	USceneComponent* parent = actor->FindComponentByClass<USceneComponent>();

	LoadContent* loadContent = LoadContent::GetInstance();
	FString materialPathDesk = loadContent->Material("wood");
	FString materialPathLegs = loadContent->Material("black");
	FString meshPath = loadContent->Mesh("cube");
	modelParams.meshPath = meshPath;
	modelParams.parent = parent;

	spawnParams.Owner = actor;
	UStaticMesh* mesh = nullptr;
	float thick = 0.1;

	// Top
	location = FVector(0, 0, size.Z);
	scale = FVector(size.X, size.Y, thick);
	modelParams.materialPath = materialPathDesk;
	modelBase->CreateActor(name + "_Top", location, rotation, scale, spawnParams, modelParams);

	modelParams.materialPath = materialPathLegs;
	if (tags.Contains("LEGS")) {
		float legThick = thick;
		float offset = thick / 2.0;
		float legOff = 0.0;
		FVector legSize = FVector(offset, offset, (size.Z / 2.0));

		for (int ii = 0; ii < 2; ii++) {
			float offXY = (offset + (0.5 * legThick) + (0.5 * legOff));
			float legZ = legSize.Z * (float)ii;

			// Left Front Leg
			location = FVector(((-0.5 * size.X) + offXY), ((0.5 * size.Y) - offXY), legZ);
			scale = FVector(legThick, legThick, legSize.Z);
			if (ii == 1) {
				modelBase->CreateActor(name + "_LegsLF1", location, rotation, scale, spawnParams, modelParams);
			} else {
				modelBase->CreateActor(name + "_LegsLF0", location, rotation, scale, spawnParams, modelParams);
			}

			// Left Back Leg
			location = FVector(((-0.5 * size.X) + offXY), ((-0.5 * size.Y) + offXY), legZ);
			scale = FVector(legThick, legThick, legSize.Z);
			if (ii == 1) {
				modelBase->CreateActor(name + "_LegsLB1", location, rotation, scale, spawnParams, modelParams);
			} else {
				modelBase->CreateActor(name + "_LegsLB0", location, rotation, scale, spawnParams, modelParams);
			}

			// Right Front Leg
			location = FVector(((0.5 * size.X) - offXY), ((0.5 * size.Y) - offXY), legZ);
			scale = FVector(legThick, legThick, legSize.Z);
			if (ii == 1) {
				modelBase->CreateActor(name + "_LegsRF1", location, rotation, scale, spawnParams, modelParams);
			} else {
				modelBase->CreateActor(name + "_LegsRF0", location, rotation, scale, spawnParams, modelParams);
			}

			// Right Back Leg
			location = FVector(((0.5 * size.X) - offXY), ((-0.5 * size.Y) + offXY), legZ);
			scale = FVector(legThick, legThick, legSize.Z);
			if (ii == 1) {
				modelBase->CreateActor(name + "_LegsRB1", location, rotation, scale, spawnParams, modelParams);
			} else {
				modelBase->CreateActor(name + "_LegsRB0", location, rotation, scale, spawnParams, modelParams);
			}

			legThick /= 2.0;
			legOff += legThick;
		} // ii

	} else {

		modelParams.materialPath = materialPathDesk;
		// Left
		location = FVector(((-0.5 * size.X) + (0.5 * thick)), 0, 0);
		scale = FVector(thick, size.Y, size.Z);
		modelBase->CreateActor(name + "_Left", location, rotation, scale, spawnParams, modelParams);

		// Right
		location = FVector(((0.5 * size.X) - (0.5 * thick)), 0, 0);
		scale = FVector(thick, size.Y, size.Z);
		modelBase->CreateActor(name + "_Right", location, rotation, scale, spawnParams, modelParams);

		// Back
		float gapBack = (0.2 * size.Z);
		location = FVector(0, ((-0.5 * size.Y) + (0.5 * thick)), gapBack);
		scale = FVector((size.X - (2.0 * thick)), thick, (size.Z - gapBack));
		modelBase->CreateActor(name + "_Back", location, rotation, scale, spawnParams, modelParams);
	} // LEGS

	if (tags.Contains("KVM")) {
		FVector defLocation = FVector(0, (-0.25 * size.Y), (size.Z + thick));
		ModelMonitor::Create(defLocation);
		defLocation = FVector((-0.10 * size.X), 0, (size.Z + thick));
		ModelKeyboard::Create(defLocation);
		defLocation = FVector((0.10 * size.X), 0, (size.Z + thick));
		ModelMouse::Create(defLocation);
	} // KVM

} // ModelDesk

/////////////////////////////////////////////////////////////////////////////////////////////
ModelMonitor::ModelMonitor() {
}

ModelMonitor::~ModelMonitor() {
}

void ModelMonitor::Create(FVector defLocation) {
	ModelBase* modelBase = ModelBase::GetInstance();
	UWorld* World = modelBase->GetWorld();
	auto [modelingBase, modelParams] = modelBase->GetInputs("Monitor1", FVector(1, 0.05, 0.8));
	FString name = modelingBase.name;
	FVector size = modelingBase.size;
	TArray<FString> tags = modelingBase.tags;

	FVector rotation = FVector(0,0,0);
	FActorSpawnParameters spawnParams;
	FVector location = defLocation; // FVector(0, 0, 0);
	FVector scale = FVector(1, 1, 1);
	AStaticMeshActor* actor;

	// Parent container
	actor = modelBase->CreateActor(name, location, rotation, scale, spawnParams);
	USceneComponent* parent = actor->FindComponentByClass<USceneComponent>();

	LoadContent* loadContent = LoadContent::GetInstance();
	FString meshPath = loadContent->Mesh("cube");
	FString materialPath = loadContent->Material("black");
	modelParams.meshPath = meshPath;
	modelParams.materialPath = materialPath;
	modelParams.parent = parent;

	spawnParams.Owner = actor;
	UStaticMesh* mesh = nullptr;
	float stand = 0.1;

	// Front
	location = FVector(0, 0, stand);
	scale = FVector(size.X, size.Y, size.Z);
	modelBase->CreateActor(name + "_Front", location, rotation, scale, spawnParams, modelParams);

	if (stand > 0) {
		// Back
		float thick = 0.05;
		location = FVector((-0.5 * stand), (-size.Y), thick);
		scale = FVector(stand, thick, (stand * 2));
		modelBase->CreateActor(name + "_Back", location, rotation, scale, spawnParams, modelParams);

		// Base
		location = FVector((-0.5 * stand), (-size.Y), 0);
		scale = FVector((stand * 3), stand, thick);
		modelBase->CreateActor(name + "_Base", location, rotation, scale, spawnParams, modelParams);

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
	auto [modelingBase, modelParams] = modelBase->GetInputs("Keyboard1", FVector(0.5, 0.25, 0.05));
	FString name = modelingBase.name;
	FVector size = modelingBase.size;
	TArray<FString> tags = modelingBase.tags;

	FVector rotation = FVector(0,0,0);
	FActorSpawnParameters spawnParams;
	FVector location = defLocation; // FVector(0, 0, 0);
	FVector scale = FVector(1, 1, 1);
	AStaticMeshActor* actor;

	// Parent container
	actor = modelBase->CreateActor(name, location, rotation, scale, spawnParams);
	USceneComponent* parent = actor->FindComponentByClass<USceneComponent>();

	LoadContent* loadContent = LoadContent::GetInstance();
	FString meshPath = loadContent->Mesh("cube");
	FString materialPath = loadContent->Material("grey");
	modelParams.meshPath = meshPath;
	modelParams.materialPath = materialPath;
	modelParams.parent = parent;

	spawnParams.Owner = actor;
	UStaticMesh* mesh = nullptr;

	// Top
	location = FVector(0, 0, 0);
	scale = FVector(size.X, size.Y, size.Z);
	modelBase->CreateActor(name + "_Top", location, rotation, scale, spawnParams, modelParams);
} // ModelKeyboard

/////////////////////////////////////////////////////////////////////////////////////////////
ModelMouse::ModelMouse() {
}

ModelMouse::~ModelMouse() {
}

void ModelMouse::Create(FVector defLocation) {
	ModelBase* modelBase = ModelBase::GetInstance();
	UWorld* World = modelBase->GetWorld();
	auto [modelingBase, modelParams] = modelBase->GetInputs("Mouse1", FVector(0.06, 0.12, 0.05));
	FString name = modelingBase.name;
	FVector size = modelingBase.size;
	TArray<FString> tags = modelingBase.tags;

	FVector rotation = FVector(0,0,0);
	FActorSpawnParameters spawnParams;
	FVector location = defLocation; // FVector(0, 0, 0);
	FVector scale = FVector(1, 1, 1);
	AStaticMeshActor* actor;

	// Parent container
	actor = modelBase->CreateActor(name, location, rotation, scale, spawnParams);
	USceneComponent* parent = actor->FindComponentByClass<USceneComponent>();

	LoadContent* loadContent = LoadContent::GetInstance();
	FString meshPath = loadContent->Mesh("cube");
	FString materialPath = loadContent->Material("blue");
	modelParams.meshPath = meshPath;
	modelParams.materialPath = materialPath;
	modelParams.parent = parent;

	spawnParams.Owner = actor;
	UStaticMesh* mesh = nullptr;

	// Top
	location = FVector(0, 0, 0);
	scale = FVector(size.X, size.Y, size.Z);
	modelBase->CreateActor(name + "_Top", location, rotation, scale, spawnParams, modelParams);
} // ModelMouse