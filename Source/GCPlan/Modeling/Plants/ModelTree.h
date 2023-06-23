#pragma once

#include "../ModelBase.h"

class ModelTree {
public:
	ModelTree();
	~ModelTree();

	static AActor* CreateFromInputs();
	static AActor* Create(FVector size, FModelParams modelParams = FModelParams(),
		FModelCreateParams createParams = FModelCreateParams());
};