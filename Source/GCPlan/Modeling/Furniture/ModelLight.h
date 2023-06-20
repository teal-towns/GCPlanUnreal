#pragma once

#include "../ModelBase.h"

class ModelLight {
public:
	ModelLight();
	~ModelLight();

	static AActor* CreateFromInputs();
	static AActor* Create(FVector size, FModelParams modelParams = FModelParams(),
		FModelCreateParams createParams = FModelCreateParams(), TArray<FString> tags = {});
};
