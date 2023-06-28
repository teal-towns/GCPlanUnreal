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
	FModelCreateParams createParamsIn, TMap<FString, FString> pairs, FString text) {
	ModelBase* modelBase = ModelBase::GetInstance();
	FString name = Lodash::GetInstanceId("Highlight_");
	FVector rotation = FVector(0,0,0), location = FVector(0,0,0), scale = FVector(1,1,1);
	AActor* actor = modelBase->CreateActorEmpty(name, modelParams);
	modelParams.parent = actor->FindComponentByClass<USceneComponent>();

	FActorSpawnParameters spawnParams;
	modelParams.materialKey = "whiteEmissive";

	float sphereFactor = 2;
	float lineFactor = 1;
	modelParams.meshKey = "sphere";
	scale = FVector(1,1,1) * sphereFactor;
	modelBase->CreateActor(name + "_sphere", location, rotation, scale, FActorSpawnParameters(), modelParams);

	modelParams.meshKey = "cube";
	// scale = FVector(0.1, text.Len() * 1.25, 0.1) * lineFactor;
	// location = FVector(0, 1 + scale.Y / 2, 2);
	// modelBase->CreateActor(name + "_lineBottom", location, rotation, scale, FActorSpawnParameters(), modelParams);
	// angled line to connect
	// scale = FVector(0.1, 1.8, 0.1) * lineFactor;
	// location = FVector(0, 0.5, 1.4);
	// rotation = FVector(-50,0,0);
	// modelBase->CreateActor(name + "_lineAngled", location, rotation, scale, FActorSpawnParameters(), modelParams);
	// rotation = FVector(0,0,0);

	// text
	scale = FVector(1,1,1);
	location = FVector(0, 1, 2.5);
	spawnParams.Name = FName(name + "_text");
	UnrealGlobal* unrealGlobal = UnrealGlobal::GetInstance();
	AActor* actorText = (AText3DActor*)modelBase->GetWorld()->SpawnActor<AText3DActor>(
		AText3DActor::StaticClass(), location * unrealGlobal->GetScale(),
		DataConvert::VectorToRotator(rotation), spawnParams);
	actorText->AttachToComponent(modelParams.parent, FAttachmentTransformRules::KeepRelativeTransform);
	actorText->SetActorScale3D(size);
	UText3DComponent* textComponent = actorText->FindComponentByClass<UText3DComponent>();
	textComponent->SetText(FText::FromString(text));
	UMaterialInterface* material = ModelBase::GetMaterial(modelParams);
	if (material) {
		textComponent->SetFrontMaterial(material);
	}

	// At end, scale parent.
	actor->SetActorScale3D(size);
	ModelBase::SetTransformFromParams(actor, createParamsIn);
	return actor;
}
