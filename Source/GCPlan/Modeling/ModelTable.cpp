#include "ModelTable.h"

#include "Engine/StaticMeshActor.h"

#include "ModelBase.h"
#include "../Common/Lodash.h"
#include "../Mesh/LoadContent.h"
#include "../ModelingStructsActor.h"

ModelTable::ModelTable() {
}

ModelTable::~ModelTable() {
}

AActor* ModelTable::Create() {
	ModelBase* modelBase = ModelBase::GetInstance();
	auto [modelingBase, modelParams] = modelBase->GetInputs("Table1", FVector(3,2,1.5));
	FString name = modelingBase.name;
	FVector size = modelingBase.size;
	TArray<FString> tags = modelingBase.tags;
	if (tags.Contains("roundShort")) {
		return RoundShort(size, tags);
	}

	FVector rotation = FVector(0,0,0);
	FActorSpawnParameters spawnParams;
	FVector location = FVector(0,0,0);
	FVector scale = FVector(1,1,1);
	AStaticMeshActor* actor;

	// Parent container
	actor = modelBase->CreateActor(name, location, rotation, scale, spawnParams);
	USceneComponent* parent = actor->FindComponentByClass<USceneComponent>();

	LoadContent* loadContent = LoadContent::GetInstance();
	FString materialPath = loadContent->Material("wood");
	FString materialPathMetal = loadContent->Material("metalChrome");
	FString meshPath = loadContent->Mesh("cube");
	FString meshPathCylinder = loadContent->Mesh("cylinder");
	modelParams.parent = parent;
	modelParams.meshPath = meshPath;
	modelParams.materialPath = materialPath;

	// spawnParams.Owner = actor;
	float thick = 0.1;
	float legZ = size.Z;
	float legThick = thick;
	float offset = thick / 2.0;
	float offXY = (offset + (0.5 * legThick));

	if (tags.Contains("METAL")) {
		modelParams.materialPath = materialPathMetal;
		modelParams.meshPath = meshPathCylinder;
	}

	// Left Front Leg
	location = FVector(((-0.5 * size.X) + offXY), ((0.5 * size.Y) - offXY), 0);
	scale = FVector(legThick, legThick, legZ);
	modelBase->CreateActor(name + "_LegsLF", location, rotation, scale, spawnParams, modelParams);

	// Left Back Leg
	location = FVector(((-0.5 * size.X) + offXY), ((-0.5 * size.Y) + offXY), 0);
	scale = FVector(legThick, legThick, legZ);
	modelBase->CreateActor(name + "_LegsLB", location, rotation, scale, spawnParams, modelParams);

	// Right Front Leg
	location = FVector(((0.5 * size.X) - offXY), ((0.5 * size.Y) - offXY), 0);
	scale = FVector(legThick, legThick, legZ);
	modelBase->CreateActor(name + "_LegsRF", location, rotation, scale, spawnParams, modelParams);

	// Right Back Leg
	location = FVector(((0.5 * size.X) - offXY), ((-0.5 * size.Y) + offXY), 0);
	scale = FVector(legThick, legThick, legZ);
	modelBase->CreateActor(name + "_LegsRB", location, rotation, scale, spawnParams, modelParams);

	modelParams.meshPath = meshPath;
	modelParams.materialPath = materialPath;
	// Top
	location = FVector(0, 0, legZ);
	scale = FVector(size.X, size.Y, thick);
	modelBase->CreateActor(name + "_Top", location, rotation, scale, spawnParams, modelParams);
	return actor;
}

AActor* ModelTable::RoundShort(FVector size, TArray<FString> tags, FModelParams modelParams) {
	FString name = Lodash::GetInstanceId("TableRoundShort_");
	ModelBase* modelBase = ModelBase::GetInstance();
	AActor* actor = modelBase->CreateActor(name);
	FVector scale = FVector(1,1,1), rotation = FVector(0,0,0), location = FVector(0,0,0);
	FActorSpawnParameters spawnParams;
	modelParams.parent = actor->FindComponentByClass<USceneComponent>();

	float topHeight = 0.05;
	// Legs: 2 crossed pieces.
	modelParams.meshKey = "cube";
	modelParams.materialKey = "metalChrome";
	scale = FVector(size.X * 0.9, 0.1, size.Z - topHeight);
	modelBase->CreateActor(name + "_Leg1", location, rotation, scale, spawnParams, modelParams);
	scale = FVector(size.Y * 0.9, 0.1, size.Z - topHeight);
	modelBase->CreateActor(name + "_Leg2", location, FVector(0,0,90), scale, spawnParams, modelParams);
	// Top
	modelParams.meshKey = "cylinder";
	modelParams.materialKey = "white";
	scale = FVector(size.X, size.Y, topHeight);
	modelBase->CreateActor(name + "_Top", FVector(0,0,size.Z - topHeight), rotation, scale, spawnParams, modelParams);
	return actor;
}
