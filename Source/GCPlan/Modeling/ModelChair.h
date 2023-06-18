#pragma once

#include "ModelBase.h"

class ModelChair {
public:
	ModelChair();
	~ModelChair();

	static AActor* Create();
	static AActor* UChair(FVector size, TArray<FString> tags = {},
		FModelParams modelParams = FModelParams());
};
