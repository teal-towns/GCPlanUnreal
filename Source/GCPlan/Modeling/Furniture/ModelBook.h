#pragma once

#include "../ModelBase.h"

class ModelBook {
public:
	ModelBook();
	~ModelBook();

	static AActor* CreateFromInputs();
	static AActor* Create(FVector size, FModelParams modelParams = FModelParams(),
		FModelCreateParams createParams = FModelCreateParams(), TArray<FString> tags = {});
	static AActor* Books(FVector size, FModelParams modelParams = FModelParams(),
		FModelCreateParams createParams = FModelCreateParams(), TArray<FString> tags = {});
};
