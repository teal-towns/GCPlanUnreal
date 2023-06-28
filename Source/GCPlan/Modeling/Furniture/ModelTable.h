#pragma once

#include "../ModelBase.h"

class ModelTable {
public:
	ModelTable();
	~ModelTable();

	static void Build(TMap<FString, FString> pairs);
	static AActor* CreateFromInputs();
	static AActor* Create(FVector size, FModelParams modelParams = FModelParams(),
		FModelCreateParams createParams = FModelCreateParams(), TMap<FString, FString> pairs = {});
	static AActor* RoundShort(FVector size, FModelParams modelParams = FModelParams(),
		FModelCreateParams createParams = FModelCreateParams(), TMap<FString, FString> pairs = {});
};
