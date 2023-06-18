#pragma once

#include "ModelBase.h"

class ModelTable {
public:
	ModelTable();
	~ModelTable();

	static AActor* Create();
	static AActor* RoundShort(FVector size, TArray<FString> tags = {},
		FModelParams modelParams = FModelParams());
};
