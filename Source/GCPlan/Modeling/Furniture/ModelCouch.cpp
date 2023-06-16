#include "ModelCouch.h"

#include "Engine/StaticMeshActor.h"

#include "../ModelBase.h"
#include "../Common/ModelLeg.h"
#include "../Common/ModelSide.h"
#include "../../Mesh/DynamicMaterial.h"
#include "../../Mesh/LoadContent.h"
#include "../../ModelingStructsActor.h"
#include "../../ProceduralModel/PMCylinder.h"

ModelCouch::ModelCouch() {
}

ModelCouch::~ModelCouch() {
}

void ModelCouch::Create() {
	ModelBase* modelBase = ModelBase::GetInstance();
	FModelingBase modelingBase = modelBase->GetInputs("Couch1", FVector(1.3,2.3,1));
	FString name = modelingBase.name;
	FVector size = modelingBase.size;
	TArray<FString> tags = modelingBase.tags;
	int cushionCount = 2;
	if (tags.Contains("3Cushions")) {
		cushionCount = 3;
	}

	FRotator rotation = FRotator(0,0,0);
	FActorSpawnParameters spawnParams;
	FVector location = FVector(0,0,0);
	FVector scale = FVector(1,1,1);
	AStaticMeshActor* actor;

	// Parent container
	actor = modelBase->CreateActor(name, location, rotation, scale, spawnParams);
	USceneComponent* parent = actor->FindComponentByClass<USceneComponent>();

	LoadContent* loadContent = LoadContent::GetInstance();
	DynamicMaterial* dynamicMaterial = DynamicMaterial::GetInstance();
	FString meshCube = loadContent->Mesh("cube");
	FString materialPath = loadContent->Material("dynamicLeather");
	FString texturePathBase = loadContent->Texture("leather_base");
	FString texturePathNormal = loadContent->Texture("leather_normal");
	// FString texturePathBase = loadContent->Texture("marble_base");
	// FString texturePathNormal = loadContent->Texture("marble_normal");
	// UMaterialInstanceDynamic* material = dynamicMaterial->CreateTextureColor(name + "_leather", texturePathBase,
	// 	texturePathNormal, FLinearColor(255,230,145));
	UMaterialInstanceDynamic* material = dynamicMaterial->CreateTexture(name + "_leather", texturePathBase,
		texturePathNormal);
	FString materialPathWood = loadContent->Material("wood");
	FModelParams modelParams;
	modelParams.meshPath = meshCube;
	modelParams.parent = parent;

	modelParams.materialPath = materialPathWood;
	FVector scaleLeg = FVector(0.1, 0.1, 0.2);
	float buffer = 0.2;
	ModelLeg::FrontRight(name, size, scaleLeg, buffer, modelParams);
	ModelLeg::BackRight(name, size, scaleLeg, buffer, modelParams);
	ModelLeg::BackLeft(name, size, scaleLeg, buffer, modelParams);
	ModelLeg::FrontLeft(name, size, scaleLeg, buffer, modelParams);

	// modelParams.materialPath = materialPath;
	modelParams.dynamicMaterial = material;
	FVector scaleSide = FVector(0.1, 0.2, 0.2);
	FVector sizeSide = FVector(size.X, size.Y, size.Z - scaleLeg.Z);
	FVector offsetSide = FVector(0,0,scaleLeg.Z);
	ModelSide::Back(name, sizeSide, scaleSide, modelParams, offsetSide);
	ModelSide::Right(name, sizeSide, scaleSide, modelParams, offsetSide);
	ModelSide::Left(name, sizeSide, scaleSide, modelParams, offsetSide);
	ModelSide::Bottom(name, size, scaleSide, modelParams, offsetSide);

	// Cushions
	FVector spacing = FVector(0.02, 0.02, 0.02);
	// Spacing on ends and between each cushion.
	float totalSpacingY = spacing.Y * (cushionCount + 1);
	// Bottom cushions go along bottom (under back cushions).
	FVector sizeCushionsBottom = FVector(size.X - scaleSide.X, size.Y - 2 * scaleSide.Y - totalSpacingY, 0.2);
	FVector scaleCushionBottom = FVector(sizeCushionsBottom.X,
		sizeCushionsBottom.Y / cushionCount, sizeCushionsBottom.Z);
	FVector locationBottom = FVector(scaleSide.X / 2,
		-1 * size.Y / 2 + scaleSide.Y + scaleCushionBottom.Y / 2,
		scaleLeg.Z + scaleSide.Z);

	// Back cushions go on top of bottom cushions.
	FVector sizeCushionsBack = FVector(0.2, size.Y - 2 * scaleSide.Y - totalSpacingY,
		size.Z - scaleLeg.Z - scaleSide.Z - scaleCushionBottom.Z);
	FVector scaleCushionBack = FVector(sizeCushionsBack.X,
		sizeCushionsBack.Y / cushionCount, sizeCushionsBack.Z);
	// Go left (-Y) to right
	FVector locationBack = FVector(-1 * size.X / 2 + scaleSide.X + scaleCushionBack.X / 2,
		-1 * size.Y / 2 + scaleSide.Y + scaleCushionBack.Y / 2,
		locationBottom.Z + scaleCushionBottom.Z);

	locationBack += spacing;
	locationBottom += spacing;
	// Extra to account for bottom spacing.
	locationBack.Z += spacing.Z;
	for (int ii = 0; ii < cushionCount; ii++) {
		modelBase->CreateActor(name + "_CushionBack" + FString::FromInt(ii), locationBack, FRotator(0,0,0), scaleCushionBack,
			spawnParams, modelParams);
		modelBase->CreateActor(name + "_CushionBottom" + FString::FromInt(ii), locationBottom, FRotator(0,0,0), scaleCushionBottom,
			spawnParams, modelParams);
		// Move Y over to next cushion.
		locationBack.Y += spacing.Y + scaleCushionBack.Y;
		locationBottom.Y += spacing.Y + scaleCushionBottom.Y;
	}
}
