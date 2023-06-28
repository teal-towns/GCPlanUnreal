#pragma once

#include "../ModelBase.h"

class ModelHighlight {
public:
	ModelHighlight();
	~ModelHighlight();

	static AActor* Create(FVector size, FModelParams modelParams = FModelParams(),
		FModelCreateParams createParams = FModelCreateParams(), TMap<FString, FString> pairs = {},
		FString text = "text");
};
