#include "ModelTable.h"

#include "Engine/StaticMeshActor.h"

#include "ModelBase.h"
#include "../Mesh/LoadContent.h"
#include "../ModelingStructsActor.h"
#include "../ProceduralModel/PMCylinder.h"

/////////////////////////////////////////////////////////////////////////////////////////////
ModelTable::ModelTable() {
}

ModelTable::~ModelTable() {
}

void ModelTable::Create() {
	ModelBase* modelBase = ModelBase::GetInstance();
	UWorld* World = modelBase->GetWorld();
	FModelingBase modelingBase = modelBase->GetInputs("Table1", FVector(3,2,1.5));
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
	FString materialPath = loadContent->Material("wood");
	FString materialPathMetal = loadContent->Material("black");
	FString meshPath = "/Script/Engine.StaticMesh'/Game/Modeling/Primitives/Cube.Cube'";
	FString meshPathCylinder = "/Script/Engine.StaticMesh'/Game/Modeling/Primitives/Cylinder.Cylinder'";

	spawnParams.Owner = actor;
	UStaticMesh* mesh = nullptr;
	float thick = 0.1;
	float legZ = size.Z;
	float legThick = thick;
	float offset = thick / 2.0;
	float offXY = (offset + (0.5 * legThick));

	// Left Front Leg
	location = FVector(((-0.5 * size.X) + offXY), ((0.5 * size.Y) - offXY), 0);
	scale = FVector(legThick, legThick, legZ);
	if (tags.Contains("METAL")) {
		modelBase->CreateActor(name + "_LegsLF", location, rotation, scale, spawnParams, parent, meshPathCylinder, materialPathMetal);
	} else {
		modelBase->CreateActor(name + "_LegsLF", location, rotation, scale, spawnParams, parent, meshPath, materialPath);
	} // METAL

	// Left Back Leg
	location = FVector(((-0.5 * size.X) + offXY), ((-0.5 * size.Y) + offXY), 0);
	scale = FVector(legThick, legThick, legZ);
	if (tags.Contains("METAL")) {
		modelBase->CreateActor(name + "_LegsLB", location, rotation, scale, spawnParams, parent, meshPathCylinder, materialPathMetal);
	} else {
		modelBase->CreateActor(name + "_LegsLB", location, rotation, scale, spawnParams, parent, meshPath, materialPath);
	} // METAL

	// Right Front Leg
	location = FVector(((0.5 * size.X) - offXY), ((0.5 * size.Y) - offXY), 0);
	scale = FVector(legThick, legThick, legZ);
	if (tags.Contains("METAL")) {
		modelBase->CreateActor(name + "_LegsRF", location, rotation, scale, spawnParams, parent, meshPathCylinder, materialPathMetal);
	} else {
		modelBase->CreateActor(name + "_LegsRF", location, rotation, scale, spawnParams, parent, meshPath, materialPath);
	} // METAL

	// Right Back Leg
	location = FVector(((0.5 * size.X) - offXY), ((-0.5 * size.Y) + offXY), 0);
	scale = FVector(legThick, legThick, legZ);
	if (tags.Contains("METAL")) {
		modelBase->CreateActor(name + "_LegsRB", location, rotation, scale, spawnParams, parent, meshPathCylinder, materialPathMetal);
	} else {
		modelBase->CreateActor(name + "_LegsRB", location, rotation, scale, spawnParams, parent, meshPath, materialPath);
	} // METAL

	// Top
	location = FVector(0, 0, legZ);
	scale = FVector(size.X, size.Y, thick);
	modelBase->CreateActor(name + "_Top", location, rotation, scale, spawnParams, parent, meshPath, materialPath);

} // ModelTable

