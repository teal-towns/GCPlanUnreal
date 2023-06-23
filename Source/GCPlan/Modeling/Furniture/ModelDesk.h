#pragma once

#include "../ModelBase.h"

struct FModelDesk {
	float woodThickness = 0.05;
	float bottomLegMinHeight = 0.67;
};

class ModelDesk {
public:
	ModelDesk();
	~ModelDesk();

	static void Build(TMap<FString, FString> pairs);
	static AActor* CreateFromInputs();
	static AActor* Create(FVector size, FModelParams modelParams = FModelParams(),
		FModelCreateParams createParams = FModelCreateParams(), TMap<FString, FString> pairs = {},
		FModelDesk params = FModelDesk());
};
