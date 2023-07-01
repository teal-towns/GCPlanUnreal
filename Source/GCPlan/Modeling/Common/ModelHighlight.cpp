#include "ModelHighlight.h"

#include "Text3DActor.h"
#include "Text3DComponent.h"

#include "../ModelBase.h"
#include "../../Common/DataConvert.h"
#include "../../Common/Lodash.h"
#include "../../Common/UnrealGlobal.h"
#include "../../Mesh/LoadContent.h"
#include "../../ModelingStructsActor.h"

ModelHighlight::ModelHighlight() {
}

ModelHighlight::~ModelHighlight() {
}

AActor* ModelHighlight::Create(FVector size, FModelParams modelParams,
	FModelCreateParams createParamsIn, TMap<FString, FString> pairs, FHighlightText params) {
	ModelBase* modelBase = ModelBase::GetInstance();
	FString name = Lodash::GetInstanceId("Highlight_");
	FVector rotation = FVector(0,0,0), location = FVector(0,0,0), scale = FVector(1,1,1);
	AActor* actor = modelBase->CreateActorEmpty(name, modelParams);
	modelParams.parent = actor->FindComponentByClass<USceneComponent>();

	FActorSpawnParameters spawnParams;
	modelParams.materialKey = "whiteEmissive";

	UE_LOG(LogTemp, Display, TEXT("dotMeshKey %s"), *params.dotMeshKey);
	modelParams.meshKey = params.dotMeshKey;
	scale = params.dotScale;
	rotation = params.dotRotation;
	modelBase->CreateActor(name + "_dot", location, rotation, scale, FActorSpawnParameters(), modelParams);

	modelParams.meshKey = "cube";
	// scale = FVector(0.1, params.text.Len() * 1.25, 0.1) * params.lineFactor;
	// location = FVector(0, 1 + scale.Y / 2, 2);
	// modelBase->CreateActor(name + "_lineBottom", location, rotation, scale, FActorSpawnParameters(), modelParams);
	// angled line to connect
	// scale = FVector(0.1, 1.8, 0.1) * params.lineFactor;
	// location = FVector(0, 0.5, 1.4);
	// rotation = FVector(-50,0,0);
	// modelBase->CreateActor(name + "_lineAngled", location, rotation, scale, FActorSpawnParameters(), modelParams);
	// rotation = FVector(0,0,0);

	UnrealGlobal* unrealGlobal = UnrealGlobal::GetInstance();
	AActor* actorText;
	UText3DComponent* textComponent;
	UMaterialInterface* material;
	// text
	scale = FVector(1,1,1);
	location = params.textOffset;
	rotation = params.textRotation;
	spawnParams.Name = FName(name + "_text");
	actorText = (AText3DActor*)modelBase->GetWorld()->SpawnActor<AText3DActor>(
		AText3DActor::StaticClass(), location * unrealGlobal->GetScale(),
		DataConvert::VectorToRotator(rotation), spawnParams);
	actorText->AttachToComponent(modelParams.parent, FAttachmentTransformRules::KeepRelativeTransform);
	actorText->SetActorScale3D(size * scale);
	textComponent = actorText->FindComponentByClass<UText3DComponent>();
	textComponent->SetText(FText::FromString(params.text));
	if (params.textAlignment == "right") {
		textComponent->SetHorizontalAlignment(EText3DHorizontalTextAlignment::Right);
	} else if (params.textAlignment == "center") {
		textComponent->SetHorizontalAlignment(EText3DHorizontalTextAlignment::Center);
	}
	material = ModelBase::GetMaterial(modelParams);
	if (material) {
		textComponent->SetFrontMaterial(material);
	}

	// sub-text
	if (params.subText.Len() > 0) {
		scale = params.subTextScale;
		location = params.subTextOffset;
		rotation = params.textRotation;
		spawnParams.Name = FName(name + "_subText");
		actorText = (AText3DActor*)modelBase->GetWorld()->SpawnActor<AText3DActor>(
			AText3DActor::StaticClass(), location * unrealGlobal->GetScale(),
			DataConvert::VectorToRotator(rotation), spawnParams);
		actorText->AttachToComponent(modelParams.parent, FAttachmentTransformRules::KeepRelativeTransform);
		actorText->SetActorScale3D(size * scale);
		textComponent = actorText->FindComponentByClass<UText3DComponent>();
		textComponent->SetText(FText::FromString(params.subText));
		if (params.textAlignment == "right") {
			textComponent->SetHorizontalAlignment(EText3DHorizontalTextAlignment::Right);
		} else if (params.textAlignment == "center") {
			textComponent->SetHorizontalAlignment(EText3DHorizontalTextAlignment::Center);
		}
		material = ModelBase::GetMaterial(modelParams);
		if (material) {
			textComponent->SetFrontMaterial(material);
		}
	}

	// At end, scale parent.
	actor->SetActorScale3D(size);
	// If do this at start, get warning that cannot attach static mesh to movable parent.
	// if (modelParams.movable) {
	// 	USceneComponent* component = actor->FindComponentByClass<USceneComponent>();
	// 	component->SetMobility(EComponentMobility::Movable);
	// }
	ModelBase::SetTransformFromParams(actor, createParamsIn, modelParams);
	return actor;
}
