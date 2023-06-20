#include "ModelChair.h"

#include "Engine/StaticMeshActor.h"

#include "../ModelBase.h"
#include "../../Common/Lodash.h"
#include "../../Mesh/DynamicMaterial.h"
#include "../../Mesh/LoadContent.h"
#include "../../Modeling/Common/ModelLeg.h"
#include "../../ModelingStructsActor.h"
#include "../../ProceduralModel/PMPlaneU.h"

ModelChair::ModelChair() {
}

ModelChair::~ModelChair() {
}

AActor* ModelChair::CreateFromInputs() {
	ModelBase* modelBase = ModelBase::GetInstance();
	UWorld* World = modelBase->GetWorld();
	auto [modelingBase, modelParams] = modelBase->GetInputs("Chair1", FVector(0.5,0.5,1));
	FString name = modelingBase.name;
	FVector size = modelingBase.size;
	TArray<FString> tags = modelingBase.tags;
	if (tags.Contains("uChair")) {
		return UChair(size, modelParams, FModelCreateParams(), tags);
	}
	return Create(size, modelParams, FModelCreateParams(), tags);
}

AActor* ModelChair::Create(FVector size, FModelParams modelParams,
	FModelCreateParams createParamsIn, TArray<FString> tags) {
	ModelBase* modelBase = ModelBase::GetInstance();
	FString name = Lodash::GetInstanceId("Chair_");
	FVector rotation = FVector(0,0,0), location = FVector(0,0,0), scale = FVector(1,1,1);
	AActor* actor = modelBase->CreateActorEmpty(name, modelParams);
	USceneComponent* parent = actor->FindComponentByClass<USceneComponent>();

	modelParams.parent = parent;
	modelParams.meshKey = "cube";
	modelParams.materialKey = "wood";
	FActorSpawnParameters spawnParams;
	float seatHeight = 0.5;
	float seatThickness = 0.05;
	float legsOffset = 0.05;
	if (tags.Contains("metal")) {
		modelParams.materialKey = "metalChrome";
	}

	modelParams.meshKey = "cube";
	modelParams.materialKey = "wood";
	if (tags.Contains("stool")) {
		seatHeight = seatThickness;
		legsOffset = 0.1;
		modelParams.meshKey = "cylinder";
		modelParams.materialKey = "metalChrome";
	}
	FVector scaleLeg = FVector(0.05, 0.05, size.Z - seatHeight);
	ModelLeg::FrontRight(name, size, scaleLeg, legsOffset, modelParams);
	ModelLeg::BackRight(name, size, scaleLeg, legsOffset, modelParams);
	ModelLeg::BackLeft(name, size, scaleLeg, legsOffset, modelParams);
	ModelLeg::FrontLeft(name, size, scaleLeg, legsOffset, modelParams);

	if (tags.Contains("stool")) {
		location = FVector(0, 0, size.Z - seatHeight);
		if (tags.Contains("squareSeat")) {
			scale = FVector(size.X, size.Y, seatThickness);
			modelParams.meshKey = "cube";
			modelBase->CreateActor(name + "_Seat", location, rotation, scale, spawnParams, modelParams);
		} else {
			modelParams.meshKey = "cylinder";
			float roundXY = 1.3;
			scale = FVector((size.X * roundXY), (size.Y * roundXY), seatThickness);
			modelBase->CreateActor(name + "_Seat", location, rotation, scale, spawnParams, modelParams);
		}

	} else {
		modelParams.meshKey = "cube";
		modelParams.materialKey = "wood";
		float armThickness = seatThickness;
		float seatZ = size.Z - seatHeight;
		// Seat
		location = FVector(0, 0, size.Z - seatHeight);
		scale = FVector(size.X, size.Y, seatThickness);
		modelBase->CreateActor(name + "_Seat", location, rotation, scale, spawnParams, modelParams);

		// Back
		scale = FVector(seatThickness, size.Y, size.Z - seatHeight);
		location = FVector(-0.5 * size.X + scale.X / 2, 0, seatZ);
		modelBase->CreateActor(name + "_Back", location, rotation, scale, spawnParams, modelParams);

		float armZ = seatHeight * 0.4;
		scale = FVector(size.X - armThickness, armThickness, armThickness);
		// Left ArmRest
		location = FVector(armThickness / 2, -1 * size.Y / 2 + scale.Y / 2, seatZ + armZ);
		modelBase->CreateActor(name + "_ArmsLY", location, rotation, scale, spawnParams, modelParams);
		// Right ArmRest
		location = FVector(armThickness / 2, size.Y / 2 - scale.Y / 2, seatZ + armZ);
		modelBase->CreateActor(name + "_ArmsRY", location, rotation, scale, spawnParams, modelParams);

		scale = FVector(armThickness, armThickness, armZ);
		// Left ArmZ
		location = FVector(size.X / 2 - scale.X / 2, size.Y / -2 + scale.Y / 2, seatZ);
		modelBase->CreateActor(name + "_ArmsLZ", location, rotation, scale, spawnParams, modelParams);
		// Right ArmZ
		location = FVector(size.X / 2 - scale.X / 2, size.Y / 2 - scale.Y / 2, seatZ);
		modelBase->CreateActor(name + "_LegsRZ", location, rotation, scale, spawnParams, modelParams);
	}

	ModelBase::SetTransformFromParams(actor, createParamsIn);
	return actor;
}

AActor* ModelChair::UChair(FVector size, FModelParams modelParams, FModelCreateParams createParamsIn,
	TArray<FString> tags) {
	float seatHeight = 0.5;
	float seatThickness = 0.05;

	ModelBase* modelBase = ModelBase::GetInstance();
	FString name = Lodash::GetInstanceId("UChair_ir_");
	FVector rotation = FVector(0,0,0), location = FVector(0,0,0), scale = FVector(1,1,1);
	AActor* actor = modelBase->CreateActorEmpty(name, modelParams);
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

	ModelBase::SetTransformFromParams(actor, createParamsIn);
	return actor;
}
