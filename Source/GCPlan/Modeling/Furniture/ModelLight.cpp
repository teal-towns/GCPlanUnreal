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

void ModelLight::Build(TMap<FString, FString> pairs) {
	auto [key, modelParams] = ModelBase::ModelParamsFromPairs(pairs);
	auto [location, rotation, scale] = ModelBase::PairsToTransform(pairs, FVector(0,0,0));
	FModelCreateParams createParams;
	createParams.offset = location;
	createParams.rotation = rotation;
	if (!pairs.Contains("meshRule")) {
		pairs.Add("meshRule", "");
	}
	Create(scale, modelParams, createParams, pairs);
}

AActor* ModelLight::CreateFromInputs() {
	ModelBase* modelBase = ModelBase::GetInstance();
	auto [modelingBase, modelParams] = modelBase->GetInputs("Light", FVector(0.3,0.3,1));
	FString name = modelingBase.name;
	FVector size = modelingBase.size;
	return Create(size, modelParams, FModelCreateParams());
}

AActor* ModelLight::Create(FVector size, FModelParams modelParams,
	FModelCreateParams createParamsIn, TMap<FString, FString> pairs) {
	ModelBase* modelBase = ModelBase::GetInstance();
	FString name = Lodash::GetInstanceId("Light_");
	FVector rotation = FVector(0,0,0), location = FVector(0,0,0), scale = FVector(1,1,1);
	AActor* actor = modelBase->CreateActorEmpty(name, modelParams);
	modelParams.parent = actor->FindComponentByClass<USceneComponent>();

	FActorSpawnParameters spawnParams;
	float lightHeight = size.X;

	DynamicMaterial* dynamicMaterial = DynamicMaterial::GetInstance();
	FLinearColor color = FLinearColor(Lodash::RandomRangeFloat(0,1), Lodash::RandomRangeFloat(0,1),
		Lodash::RandomRangeFloat(0,1));
	modelParams.dynamicMaterial = dynamicMaterial->CreateColor(name + "_Cord", FLinearColor(0.5,0.5,0.5,1));

	float attachHeight = 0.05;
	float attachZ = size.Z - attachHeight;
	modelParams.meshKey = "cylinder";
	modelBase->CreateActor(name + "_Attachment", FVector(0,0,attachZ), rotation,
		FVector(0.1, 0.1, attachHeight), spawnParams, modelParams);

	float cordHeight = size.Z - attachHeight - lightHeight;
	// Cord
	modelParams.meshKey = "cube";
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

	ModelBase::SetTransformFromParams(actor, createParamsIn);
	return actor;
}
