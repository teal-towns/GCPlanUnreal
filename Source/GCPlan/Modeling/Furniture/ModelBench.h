#pragma once

#include "../ModelBase.h"

class ModelBench {
public:
	ModelBench();
	~ModelBench();

	static AActor* CreateFromInputs();
	static AActor* Create(FVector size, FModelParams modelParams = FModelParams(),
		FModelCreateParams createParams = FModelCreateParams(),
		TMap<FString, FString> pairs = {});
};
