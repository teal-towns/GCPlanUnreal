#pragma once

#include "../ModelBase.h"

class ModelCord {
public:
	ModelCord();
	~ModelCord();

	static void Build(TMap<FString, FString> pairs);
	static AActor* Create(FVector size, FModelParams modelParams = FModelParams(),
		FModelCreateParams createParams = FModelCreateParams(), TMap<FString, FString> pairs = {});
};
