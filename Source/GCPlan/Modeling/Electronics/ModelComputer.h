#pragma once

#include "../ModelBase.h"

class ModelComputer {
public:
	ModelComputer();
	~ModelComputer();

	static AActor* Keyboard(FVector size, FModelParams modelParams = FModelParams(),
		FModelCreateParams createParams = FModelCreateParams());
	static AActor* Mouse(FVector size, FModelParams modelParams = FModelParams(),
		FModelCreateParams createParams = FModelCreateParams());
};