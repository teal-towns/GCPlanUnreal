#pragma once

#include "../ModelBase.h"

class ModelChair {
public:
	ModelChair();
	~ModelChair();

	static AActor* CreateFromInputs();
	static AActor* Create(FVector size, FModelParams modelParams = FModelParams(),
		FModelCreateParams createParams = FModelCreateParams(),
		TMap<FString, FString> pairs = {});
	static AActor* UChair(FVector size, FModelParams modelParams = FModelParams(),
		FModelCreateParams createParams = FModelCreateParams());
};
