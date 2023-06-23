#pragma once

#include "../ModelBase.h"

class ModelPlanterBox {
public:
	ModelPlanterBox();
	~ModelPlanterBox();

	static void Build(TMap<FString, FString> pairs);
	static AActor* CreateFromInputs();
	static AActor* Create(FVector size, FModelParams modelParams = FModelParams(),
		FModelCreateParams createParams = FModelCreateParams());
	static AActor* CylinderRaised(FVector size, FModelParams modelParams = FModelParams(),
		FModelCreateParams createParams = FModelCreateParams(),
		TMap<FString, FString> pairs = {});
};
