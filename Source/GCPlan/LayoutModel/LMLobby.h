#pragma once

#include "../Modeling/ModelBase.h"

class LMLobby {
public:
	LMLobby();
	~LMLobby();

	static AActor* CreateFromInputs();
	static AActor* Create(FVector size, FModelParams modelParams = FModelParams(),
		FModelCreateParams createParamsIn = FModelCreateParams(), FString name = "");
	static AActor* CouchesCoffeeTables(FVector size, FModelParams modelParams,
		FModelCreateParams createParamsIn = FModelCreateParams());
};
