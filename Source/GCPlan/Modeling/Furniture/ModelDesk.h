#pragma once

#include "../ModelBase.h"

class ModelDesk {
public:
	ModelDesk();
	~ModelDesk();

	static AActor* CreateFromInputs();
	static AActor* Create(FVector size, FModelParams modelParams = FModelParams(),
		FModelCreateParams createParams = FModelCreateParams(), TArray<FString> tags = {},
		TMap<FString, float> dimensions = {});
};
