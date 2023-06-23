#pragma once

#include "../ModelBase.h"

class ModelBush {
public:
	ModelBush();
	~ModelBush();

	static AActor* CreateFromInputs();
	static AActor* Create(FVector size, FModelParams modelParams = FModelParams(),
		FModelCreateParams createParams = FModelCreateParams());
};