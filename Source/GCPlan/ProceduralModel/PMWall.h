#pragma once

#include "../Modeling/ModelBase.h"

struct FWall {
	TArray<TArray<FVector>> holesVertices = {};
	FString triangleDirection = "counterClockwise";
	float minX = 0;
};

class PMWall {
public:
	PMWall();
	~PMWall();

	static AActor* Create(FVector size, FModelCreateParams createParamsIn,
		FModelParams modelParams = FModelParams(), FWall params = FWall());
};
