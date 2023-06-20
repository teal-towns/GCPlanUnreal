#include "ModelChair.h"

#include "Engine/StaticMeshActor.h"

#include "ModelBase.h"
#include "../Common/Lodash.h"
#include "../Mesh/DynamicMaterial.h"
#include "../Mesh/LoadContent.h"
#include "../Modeling/Common/ModelLeg.h"
#include "../ModelingStructsActor.h"
#include "../ProceduralModel/PMPlaneU.h"

ModelChair::ModelChair() {
}

ModelChair::~ModelChair() {
}

AActor* ModelChair::Create() {
	ModelBase* modelBase = ModelBase::GetInstance();
	UWorld* World = modelBase->GetWorld();
	auto [modelingBase, modelParams] = modelBase->GetInputs("Chair1", FVector(1,1,1));
	FString name = modelingBase.name;
	FVector size = modelingBase.size;
	TArray<FString> tags = modelingBase.tags;
	if (tags.Contains("uChair")) {
		return UChair(size, tags);
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

	if (tags.Contains("STOOL")) {
		modelParams.meshPath = meshPathCylinder;
	}
	if (tags.Contains("METAL")) {
		modelParams.materialPath = materialPathMetal;
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

	if (tags.Contains("STOOL")) {
		// Left Leg Bar
		location = FVector(((-0.5 * size.X) + offXY), (0.5 * offXY), size.Z);
		scale = FVector(thick, (size.Y - (2.0 * offXY)), thick);
		modelBase->CreateActor(name + "_ArmsLC", location, rotation, scale, spawnParams, modelParams);

		// Right Leg Bar
		location = FVector(((0.5 * size.X) - offXY), (0.5 * offXY), size.Z);
		scale = FVector(thick, (size.Y - (2.0 * offXY)), thick);
		modelBase->CreateActor(name + "_ArmsRC", location, rotation, scale, spawnParams, modelParams);

		// Front Leg Bar
		location = FVector((0.5 * offXY), ((0.5 * size.Y) - offXY), size.Z);
		scale = FVector((size.X - (2.0 * offXY)), thick, thick);
		modelBase->CreateActor(name + "_ArmsFC", location, rotation, scale, spawnParams, modelParams);

		// Back Leg Bar
		location = FVector((0.5 * offXY), ((-0.5 * size.Y) + offXY), size.Z);
		scale = FVector((size.X - (2.0 * offXY)), thick, thick);
		modelBase->CreateActor(name + "_ArmsBC", location, rotation, scale, spawnParams, modelParams);

		// Seat 
		location = FVector(0, 0, legZ);
		if (tags.Contains("SQUARESEAT")) {
			scale = FVector(size.X, size.Y, thick);
			modelBase->CreateActor(name + "_Seat", location, rotation, scale, spawnParams, modelParams);
		} else {
			modelParams.meshPath = meshPathCylinder;
			float roundXY = 1.3;
			scale = FVector((size.X * roundXY), (size.Y * roundXY), thick);
			modelBase->CreateActor(name + "_Seat", location, rotation, scale, spawnParams, modelParams);
		}

	} else {
		modelParams.meshPath = meshPath;
		modelParams.materialPath = materialPath;
		// Seat
		location = FVector(0, 0, size.Z);
		scale = FVector(size.X, size.Y, thick);
		modelBase->CreateActor(name + "_Seat", location, rotation, scale, spawnParams, modelParams);

		// Back
		location = FVector(0, (-0.5 * size.Y), size.Z);
		scale = FVector(size.X, thick, size.Z);
		modelBase->CreateActor(name + "_Back", location, rotation, scale, spawnParams, modelParams);

		float armZ = size.Z * 0.3;
		// Left ArmRest
		location = FVector(((-0.5 * size.X) - (0.5 * thick)), -offXY, (size.Z + armZ));
		scale = FVector(thick, (size.Y - offXY), thick);
		modelBase->CreateActor(name + "_ArmsLY", location, rotation, scale, spawnParams, modelParams);

		// Right ArmRest
		location = FVector(((0.5 * size.X) + (0.5 * thick)), -offXY, (size.Z + armZ));
		scale = FVector(thick, (size.Y - offXY), thick);
		modelBase->CreateActor(name + "_ArmsRY", location, rotation, scale, spawnParams, modelParams);

		// Left ArmZ
		location = FVector(((-0.5 * size.X) - (0.5 * thick)), ((0.5 * size.Y) - (2.0 * offXY)), size.Z);
		scale = FVector(thick, thick, (armZ + thick));
		modelBase->CreateActor(name + "_ArmsLZ", location, rotation, scale, spawnParams, modelParams);

		// Right FrontZ
		location = FVector(((0.5 * size.X) + (0.5 * thick)), ((0.5 * size.Y) - (2.0 * offXY)), size.Z);
		scale = FVector(thick, thick, (armZ + thick));
		modelBase->CreateActor(name + "_LegsRZ", location, rotation, scale, spawnParams, modelParams);
	} // STOOL

	return actor;
} // ModelChair

AActor* ModelChair::UChair(FVector size, TArray<FString> tags, FModelParams modelParams) {
	float seatHeight = 0.5;
	float seatThickness = 0.05;

	FString name = Lodash::GetInstanceId("UChair_");
	ModelBase* modelBase = ModelBase::GetInstance();
	AActor* actor = modelBase->CreateActor(name);
	FVector scale = FVector(1,1,1), rotation = FVector(0,0,0), location = FVector(0,0,0);
	FActorSpawnParameters spawnParams;
	modelParams.parent = actor->FindComponentByClass<USceneComponent>();

	FVector scaleLeg = FVector(0.05, 0.05, size.Z - seatHeight);
	float buffer = 0.2;
	modelParams.meshKey = "cube";
	modelParams.materialKey = "black";
	ModelLeg::FrontRight(name, size, scaleLeg, buffer, modelParams);
	ModelLeg::BackRight(name, size, scaleLeg, buffer, modelParams);
	ModelLeg::BackLeft(name, size, scaleLeg, buffer, modelParams);
	ModelLeg::FrontLeft(name, size, scaleLeg, buffer, modelParams);

	// Seat
	FModelCreateParams createParams;
	createParams.parentActor = actor;
	createParams.parent = modelParams.parent;
	LoadContent* loadContent = LoadContent::GetInstance();
	DynamicMaterial* dynamicMaterial = DynamicMaterial::GetInstance();
	FString texturePathBase = loadContent->Texture("leather_base");
	FString texturePathNormal = loadContent->Texture("leather_normal");
	modelParams.dynamicMaterial = dynamicMaterial->CreateTextureColor(name + "_leather", texturePathBase,
		texturePathNormal, DynamicMaterial::GetColor("beige"));

	// Plane map: z, y, x
	scale = FVector(seatHeight, size.Y, size.X);
	createParams.offset = FVector(0, 0, seatHeight);
	AActor* actorTemp = PMPlaneU::Shape(name, scale, createParams, modelParams, seatThickness);
	// TODO
	// ModelBase::SetTransform(actorTemp, offsetParent + FVector(scale.X / 2, scale.Y / 2), FVector(0,90,0));

	return actor;
}
