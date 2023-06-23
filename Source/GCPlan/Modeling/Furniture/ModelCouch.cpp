#include "ModelCouch.h"

#include "Engine/StaticMeshActor.h"

#include "../ModelBase.h"
#include "../Common/ModelLeg.h"
#include "../Common/ModelSide.h"
#include "../../Common/Lodash.h"
#include "../../Mesh/DynamicMaterial.h"
#include "../../Mesh/LoadContent.h"
#include "../../ModelingStructsActor.h"
#include "../../ProceduralModel/PMCube.h"
#include "../../ProceduralModel/PMCylinder.h"

ModelCouch::ModelCouch() {
}

ModelCouch::~ModelCouch() {
}

AActor* ModelCouch::CreateFromInputs() {
	ModelBase* modelBase = ModelBase::GetInstance();
	auto [modelingBase, modelParams] = modelBase->GetInputs("Couch", FVector(1,2.3,1));
	FString name = modelingBase.name;
	FVector size = modelingBase.size;
	TMap<FString, FString> pairs = modelingBase.pairs;
	return Create(size, modelParams, FModelCreateParams(), pairs, name);
}

AActor* ModelCouch::Create(FVector size, FModelParams modelParams,
	FModelCreateParams createParamsIn, TMap<FString, FString> pairs, FString name) {
	name = ModelBase::CheckGetName(name);
	int cushionCount = pairs.Contains("3Cushions") ? 3 : 2;
	ModelBase* modelBase = ModelBase::GetInstance();
	FVector rotation = FVector(0,0,0), location = FVector(0,0,0), scale = FVector(1,1,1);
	AActor* actor = modelBase->CreateActorEmpty(name, modelParams);
	USceneComponent* parent = actor->FindComponentByClass<USceneComponent>();

	LoadContent* loadContent = LoadContent::GetInstance();
	DynamicMaterial* dynamicMaterial = DynamicMaterial::GetInstance();
	FString meshCube = loadContent->Mesh("cube");
	// FString materialPath = loadContent->Material("dynamicLeather");
	FString materialPath = loadContent->Material("leather");
	FString texturePathBase = loadContent->Texture("leather_base");
	FString texturePathNormal = loadContent->Texture("leather_normal");
	// // FString texturePathBase = loadContent->Texture("marble_base");
	// // FString texturePathNormal = loadContent->Texture("marble_normal");
	UMaterialInstanceDynamic* material = dynamicMaterial->CreateTextureColor(name + "_leather", texturePathBase,
		texturePathNormal, DynamicMaterial::GetColor("beige"));
	// UMaterialInstanceDynamic* material = dynamicMaterial->CreateTexture(name + "_leather", texturePathBase,
	// 	texturePathNormal);
	// UMaterialInstanceDynamic* material = dynamicMaterial->CreateColor(name + "_leather", FLinearColor(1,0,0,1));
	FString materialPathWood = loadContent->Material("wood");
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
	FModelParams modelParamsCushion;
	modelParamsCushion.materialPath = modelParams.materialPath;
	modelParamsCushion.dynamicMaterial = modelParams.dynamicMaterial;

	float roundedTopHeight = 0.1;
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
		// modelBase->CreateActor(name + "_CushionBack" + FString::FromInt(ii), locationBack,
		// 	FVector(0,0,0), scaleCushionBack, spawnParams, modelParams);
		// modelBase->CreateActor(name + "_CushionBottom" + FString::FromInt(ii), locationBottom,
		// 	FVector(0,0,0), scaleCushionBottom, spawnParams, modelParams);
		modelParamsCushion.location = locationBack;
		PMCube::RoundedTop(name + "_CushionBack" + FString::FromInt(ii), scaleCushionBack,
			roundedTopHeight, modelParamsCushion);
		modelParamsCushion.location = locationBottom;
		PMCube::RoundedTop(name + "_CushionBottom" + FString::FromInt(ii), scaleCushionBottom,
			roundedTopHeight, modelParamsCushion);

		// Move Y over to next cushion.
		locationBack.Y += spacing.Y + scaleCushionBack.Y;
		locationBottom.Y += spacing.Y + scaleCushionBottom.Y;
	}

	ModelBase::SetTransformFromParams(actor, createParamsIn);
	return actor;
}
