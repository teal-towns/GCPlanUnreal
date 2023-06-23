#include "ModelLeaves.h"

#include "../ModelBase.h"
#include "../../Common/Lodash.h"
#include "../../Mesh/DynamicMaterial.h"
#include "../../Mesh/LoadContent.h"
#include "../../ProceduralModel/PMPlane.h"

ModelLeaves::ModelLeaves() {
}

ModelLeaves::~ModelLeaves() {
}

AActor* ModelLeaves::CreateFromInputs() {
	ModelBase* modelBase = ModelBase::GetInstance();
	UWorld* World = modelBase->GetWorld();
	auto [modelingBase, modelParams] = modelBase->GetInputs("Leaves", FVector(0.6, 0.8, 0.7));
	FString name = modelingBase.name;
	FVector size = modelingBase.size;
	return Create(size, modelParams, FModelCreateParams());
}

AActor* ModelLeaves::Create(FVector size, FModelParams modelParams,
	FModelCreateParams createParamsIn, FModelLeaves params) {
	ModelBase* modelBase = ModelBase::GetInstance();
	FString name = Lodash::GetInstanceId("Leaves_");
	FVector rotation = FVector(0,0,0), location = FVector(0,0,0), scale = FVector(1,1,1);
	AActor* actor = modelBase->CreateActorEmpty(name, modelParams);
	modelParams.parent = actor->FindComponentByClass<USceneComponent>();
	FModelCreateParams createParams;
	createParams.parent = modelParams.parent;
	createParams.parentActor = actor;

	// LoadContent* loadContent = LoadContent::GetInstance();
	// DynamicMaterial* dynamicMaterial = DynamicMaterial::GetInstance();
	// FString texturePathBase = loadContent->Texture(params.textureBaseKey);
	// modelParams.dynamicMaterial = dynamicMaterial->CreateTexture(name + "_Leaves", texturePathBase, "");

	// TODO
	FActorSpawnParameters spawnParams;
	modelParams.meshKey = "plane";
	modelParams.materialKey = "leaves1";
	// modelParams.materialKey = "marbleTile";
	AActor* actorTemp;
	// Both sides
	createParams.triangleSide = 0;
	for (int ii = 0; ii < params.leavesCount; ii++) {
		// TESTING
		modelParams.materialKey = ii % 2 == 0 ? "blueberry" : "marbleTile";
		scale = FVector(Lodash::RandomRangeFloat(size.X / 3, size.X),
			Lodash::RandomRangeFloat(size.Z / 3, size.Z), 0.01);
		createParams.offset = FVector(0,0,0);
		createParams.rotation = FVector(0,0,0);
		actorTemp = PMPlane::Create(name + "_Leaves " + FString::FromInt(ii), scale, {}, {}, "xz", createParams, modelParams);
		createParams.offset = FVector(Lodash::RandomRangeFloat(size.X / -2 * params.offsetFactor, size.X / 2 * params.offsetFactor),
			Lodash::RandomRangeFloat(size.Y / -2 * params.offsetFactor, size.Y / 2 * params.offsetFactor), 0);
		createParams.rotation = FVector(Lodash::RandomRangeFloat(0, params.rotRangeX),
			Lodash::RandomRangeFloat(0, params.rotRangeY), Lodash::RandomRangeFloat(0, params.rotRangeZ));
		ModelBase::SetTransformFromParams(actorTemp, createParams);
	}

	ModelBase::SetTransformFromParams(actor, createParamsIn);
	return actor;
}
