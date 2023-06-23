#pragma once

#include "../ModelBase.h"

struct FModelLeaves {
	int leavesCount = 5;
	float offsetFactor = 0.5;
	float rotRangeX = 30;
	float rotRangeY = 30;
	float rotRangeZ = 360;
	FString textureBaseKey = "leaves1_base";
};

class ModelLeaves {
public:
	ModelLeaves();
	~ModelLeaves();

	static AActor* CreateFromInputs();
	static AActor* Create(FVector size, FModelParams modelParams = FModelParams(),
		FModelCreateParams createParams = FModelCreateParams(),
		FModelLeaves params = FModelLeaves());
};