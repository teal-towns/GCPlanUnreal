#pragma once

#include "../ModelBase.h"

class ModelMonitor {
public:
	ModelMonitor();
	~ModelMonitor();

	static AActor* CreateFromInputs();
	static AActor* Create(FVector size, FModelParams modelParams = FModelParams(),
		FModelCreateParams createParams = FModelCreateParams());
};