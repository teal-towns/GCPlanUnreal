#pragma once

#include "../ModelBase.h"

class ModelCouch {
public:
	ModelCouch();
	~ModelCouch();

	static AActor* CreateFromInputs();
	static AActor* Create(FVector size, FModelParams modelParams = FModelParams(),
		FModelCreateParams createParams = FModelCreateParams(), int cushionCount = 2,
		FString name = "");
};
