#include "ModelLight.h"

#include "Components/PointLightComponent.h"

#include "../ModelBase.h"
#include "../../Common/Lodash.h"
#include "../../Mesh/DynamicMaterial.h"
#include "../../ModelingStructsActor.h"

ModelLight::ModelLight() {
}

ModelLight::~ModelLight() {
}

AActor* ModelLight::Create() {
	ModelBase* modelBase = ModelBase::GetInstance();
	auto [modelingBase, modelParams] = modelBase->GetInputs("Light", FVector(0.3,0.3,1));
	FString name = modelingBase.name;
	FVector size = modelingBase.size;
	TArray<FString> tags = modelingBase.tags;

	AActor* actor = modelBase->CreateActor(name);
	modelParams.parent = actor->FindComponentByClass<USceneComponent>();

	FVector scale = FVector(1,1,1), rotation = FVector(0,0,0), location = FVector(0,0,0);
	FActorSpawnParameters spawnParams;

	float lightHeight = size.X;
	float cordHeight = size.Z - lightHeight;
	// Cord
	modelParams.meshKey = "cube";
	DynamicMaterial* dynamicMaterial = DynamicMaterial::GetInstance();
	FLinearColor color = FLinearColor(Lodash::RandomRangeFloat(0,1), Lodash::RandomRangeFloat(0,1),
		Lodash::RandomRangeFloat(0,1));
	modelParams.dynamicMaterial = dynamicMaterial->CreateColor(name + "_Cord", FLinearColor(0.5,0.5,0.5,1));
	modelBase->CreateActor(name + "_Cord", FVector(0,0,lightHeight), rotation,
		FVector(0.02, 0.02, cordHeight), spawnParams, modelParams);
	modelParams.dynamicMaterial = nullptr;

	// Light sphere
	modelParams.meshKey = "sphere";
	modelParams.materialKey = "white";
	AActor* actorTemp = modelBase->CreateActor(name + "_LightSphere", location, rotation,
		FVector(size.X, size.Y, lightHeight), spawnParams, modelParams);
	// Add light source component.
	USceneComponent* parent = actorTemp->FindComponentByClass<USceneComponent>();
	UObject* parentObject = (UObject*)actorTemp;
	UPointLightComponent* light = NewObject<UPointLightComponent>(parentObject,
		UPointLightComponent::StaticClass(), *(name + "_LightSource"));
	light->CreationMethod = EComponentCreationMethod::Instance;
	light->RegisterComponent();
	light->AttachToComponent(parent, FAttachmentTransformRules::KeepRelativeTransform);
	light->SetSourceRadius(lightHeight);

	return actor;
}
