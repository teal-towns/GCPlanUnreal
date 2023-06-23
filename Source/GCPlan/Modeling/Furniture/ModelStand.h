#pragma once

#include "../ModelBase.h"

class ModelStand {
public:
	ModelStand();
	~ModelStand();

	static AActor* CreateFromInputs();
	static AActor* Create(FVector size, FModelParams modelParams = FModelParams(),
		FModelCreateParams createParams = FModelCreateParams());
};
