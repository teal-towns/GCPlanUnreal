#include "ModelChair.h"

#include "Engine/StaticMeshActor.h"

#include "ModelBase.h"
#include "../Mesh/LoadContent.h"
#include "../ModelingStructsActor.h"
#include "../ProceduralModel/PMCylinder.h"

/////////////////////////////////////////////////////////////////////////////////////////////
ModelChair::ModelChair() {
}

ModelChair::~ModelChair() {
}

void ModelChair::Create() {
	ModelBase* modelBase = ModelBase::GetInstance();
	UWorld* World = modelBase->GetWorld();
	FModelingBase modelingBase = modelBase->GetInputs("Chair1", FVector(1,1,1));
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

	if (tags.Contains("STOOL")) {
		legZ *= 2.0;
	} // STOOL

	// Left Front Leg
	location = FVector(((-0.5 * size.X) + offXY), ((0.5 * size.Y) - offXY), 0);
	scale = FVector(legThick, legThick, legZ);
	if (tags.Contains("STOOL") && tags.Contains("METAL")) {
		modelBase->CreateActor(name + "_LegsLF", location, rotation, scale, spawnParams, parent, meshPathCylinder, materialPathMetal);
	} else {
		modelBase->CreateActor(name + "_LegsLF", location, rotation, scale, spawnParams, parent, meshPath, materialPath);
	} // METAL

	// Left Back Leg
	location = FVector(((-0.5 * size.X) + offXY), ((-0.5 * size.Y) + offXY), 0);
	scale = FVector(legThick, legThick, legZ);
	if (tags.Contains("STOOL") && tags.Contains("METAL")) {
		modelBase->CreateActor(name + "_LegsLB", location, rotation, scale, spawnParams, parent, meshPathCylinder, materialPathMetal);
	} else {
		modelBase->CreateActor(name + "_LegsLB", location, rotation, scale, spawnParams, parent, meshPath, materialPath);
	} // METAL

	// Right Front Leg
	location = FVector(((0.5 * size.X) - offXY), ((0.5 * size.Y) - offXY), 0);
	scale = FVector(legThick, legThick, legZ);
	if (tags.Contains("STOOL") && tags.Contains("METAL")) {
		modelBase->CreateActor(name + "_LegsRF", location, rotation, scale, spawnParams, parent, meshPathCylinder, materialPathMetal);
	} else {
		modelBase->CreateActor(name + "_LegsRF", location, rotation, scale, spawnParams, parent, meshPath, materialPath);
	} // METAL

	// Right Back Leg
	location = FVector(((0.5 * size.X) - offXY), ((-0.5 * size.Y) + offXY), 0);
	scale = FVector(legThick, legThick, legZ);
	if (tags.Contains("STOOL") && tags.Contains("METAL")) {
		modelBase->CreateActor(name + "_LegsRB", location, rotation, scale, spawnParams, parent, meshPathCylinder, materialPathMetal);
	} else {
		modelBase->CreateActor(name + "_LegsRB", location, rotation, scale, spawnParams, parent, meshPath, materialPath);
	} // METAL

	if (tags.Contains("STOOL")) {
		// Left Leg Bar
		location = FVector(((-0.5 * size.X) + offXY), (0.5 * offXY), size.Z);
		scale = FVector(thick, (size.Y - (2.0 * offXY)), thick);
		if (tags.Contains("METAL")) {
			modelBase->CreateActor(name + "_ArmsLC", location, rotation, scale, spawnParams, parent, meshPathCylinder, materialPathMetal);
		} else {
			modelBase->CreateActor(name + "_ArmsLC", location, rotation, scale, spawnParams, parent, meshPath, materialPath);
		} // METAL

		// Right Leg Bar
		location = FVector(((0.5 * size.X) - offXY), (0.5 * offXY), size.Z);
		scale = FVector(thick, (size.Y - (2.0 * offXY)), thick);
		if (tags.Contains("METAL")) {
			modelBase->CreateActor(name + "_ArmsRC", location, rotation, scale, spawnParams, parent, meshPathCylinder, materialPathMetal);
		} else {
			modelBase->CreateActor(name + "_ArmsRC", location, rotation, scale, spawnParams, parent, meshPath, materialPath);
		} // METAL

		// Front Leg Bar
		location = FVector((0.5 * offXY), ((0.5 * size.Y) - offXY), size.Z);
		scale = FVector((size.X - (2.0 * offXY)), thick, thick);
		if (tags.Contains("METAL")) {
			modelBase->CreateActor(name + "_ArmsFC", location, rotation, scale, spawnParams, parent, meshPathCylinder, materialPathMetal);
		} else {
			modelBase->CreateActor(name + "_ArmsFC", location, rotation, scale, spawnParams, parent, meshPath, materialPath);
		} // METAL

		// Back Leg Bar
		location = FVector((0.5 * offXY), ((-0.5 * size.Y) + offXY), size.Z);
		scale = FVector((size.X - (2.0 * offXY)), thick, thick);
		if (tags.Contains("METAL")) {
			modelBase->CreateActor(name + "_ArmsBC", location, rotation, scale, spawnParams, parent, meshPathCylinder, materialPathMetal);
		} else {
			modelBase->CreateActor(name + "_ArmsBC", location, rotation, scale, spawnParams, parent, meshPath, materialPath);
		} // METAL

		// Seat 
		location = FVector(0, 0, legZ);
#ifdef SQUARESEAT
		scale = FVector(size.X, size.Y, thick);
		modelBase->CreateActor(name + "_Seat", location, rotation, scale, spawnParams, parent, meshPath, materialPath);
#else // SQUARESEAT
		float roundXY = 1.3;
		scale = FVector((size.X * roundXY), (size.Y * roundXY), thick);
		modelBase->CreateActor(name + "_Seat", location, rotation, scale, spawnParams, parent, meshPathCylinder, materialPath);
#endif SQUARESEAT

	} else {
		// Seat
		location = FVector(0, 0, size.Z);
		scale = FVector(size.X, size.Y, thick);
		modelBase->CreateActor(name + "_Seat", location, rotation, scale, spawnParams, parent, meshPath, materialPath);

		// Back
		location = FVector(0, (-0.5 * size.Y), size.Z);
		scale = FVector(size.X, thick, size.Z);
		modelBase->CreateActor(name + "_Back", location, rotation, scale, spawnParams, parent, meshPath, materialPath);

		float armZ = size.Z * 0.3;
		// Left ArmRest
		location = FVector(((-0.5 * size.X) - (0.5 * thick)), -offXY, (size.Z + armZ));
		scale = FVector(thick, (size.Y - offXY), thick);
		modelBase->CreateActor(name + "_ArmsLY", location, rotation, scale, spawnParams, parent, meshPath, materialPath);

		// Right ArmRest
		location = FVector(((0.5 * size.X) + (0.5 * thick)), -offXY, (size.Z + armZ));
		scale = FVector(thick, (size.Y - offXY), thick);
		modelBase->CreateActor(name + "_ArmsRY", location, rotation, scale, spawnParams, parent, meshPath, materialPath);

		// Left ArmZ
		location = FVector(((-0.5 * size.X) - (0.5 * thick)), ((0.5 * size.Y) - (2.0 * offXY)), size.Z);
		scale = FVector(thick, thick, (armZ + thick));
		modelBase->CreateActor(name + "_ArmsLZ", location, rotation, scale, spawnParams, parent, meshPath, materialPath);

		// Right FrontZ
		location = FVector(((0.5 * size.X) + (0.5 * thick)), ((0.5 * size.Y) - (2.0 * offXY)), size.Z);
		scale = FVector(thick, thick, (armZ + thick));
		modelBase->CreateActor(name + "_LegsRZ", location, rotation, scale, spawnParams, parent, meshPath, materialPath);
	} // STOOL

} // ModelChair

