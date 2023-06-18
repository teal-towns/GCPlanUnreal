#pragma once

#include "../ModelBase.h"

class ModelBook {
public:
	ModelBook();
	~ModelBook();

	static AActor* Create();
	static AActor* Books(FVector size, TArray<FString> tags = {},
		FModelParams modelParams = FModelParams());
};
