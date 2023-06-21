#include "ModelPlanterBox.h"

#include "../ModelBase.h"
#include "../Common/ModelSide.h"
#include "../../Common/Lodash.h"

ModelPlanterBox::ModelPlanterBox() {
}

ModelPlanterBox::~ModelPlanterBox() {
}

void ModelPlanterBox::Build(TMap<FString, FString> pairs) {
	auto [key, modelParams] = ModelBase::ModelParamsFromPairs(pairs);
	auto [location, rotation, scale] = ModelBase::PairsToTransform(pairs, FVector(0,0,0));
	FModelCreateParams createParams;
	createParams.offset = location;
	createParams.rotation = rotation;
	if (!pairs.Contains("meshRule")) {
		pairs.Add("meshRule", "");
	}
	Create(scale, modelParams, createParams);
}

AActor* ModelPlanterBox::CreateFromInputs() {
	ModelBase* modelBase = ModelBase::GetInstance();
	auto [modelingBase, modelParams] = modelBase->GetInputs("PlanterBox", FVector(1,2.5,0.3));
	FString name = modelingBase.name;
	FVector size = modelingBase.size;
	return Create(size, modelParams, FModelCreateParams());
}

AActor* ModelPlanterBox::Create(FVector size, FModelParams modelParams,
	FModelCreateParams createParamsIn) {
	if (size == FVector(0,0,0)) {
		size = FVector(1,2.5,0.3);
	}
	FString name = Lodash::GetInstanceId("PlanterBox_");
	ModelBase* modelBase = ModelBase::GetInstance();
	FVector scale = FVector(1,1,1), rotation = FVector(0,0,0), location = FVector(0,0,0);
	AActor* actor = modelBase->CreateActorEmpty(name, modelParams);
	modelParams.parent = actor->FindComponentByClass<USceneComponent>();

	modelParams.meshKey = "cube";
	modelParams.materialKey = "wood";
	FVector scaleSide = FVector(0.05, 0.05, 0.05);
	FVector sizeSide = FVector(size.X, size.Y, size.Z);
	FVector offsetSide = FVector(0,0,0);
	ModelSide::Front(name, sizeSide, scaleSide, modelParams, offsetSide);
	ModelSide::Back(name, sizeSide, scaleSide, modelParams, offsetSide);
	ModelSide::Right(name, sizeSide, scaleSide, modelParams, offsetSide);
	ModelSide::Left(name, sizeSide, scaleSide, modelParams, offsetSide);
	ModelSide::Bottom(name, sizeSide, scaleSide, modelParams, offsetSide);
	modelParams.materialKey = "soil";
	ModelSide::Top(name, sizeSide, scaleSide, modelParams, FVector(0,0,-0.05));

	ModelBase::SetTransformFromParams(actor, createParamsIn);
	return actor;
}
