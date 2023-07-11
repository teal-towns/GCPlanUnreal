#pragma once

#include "../ModelBase.h"

struct FModelBuilding {
	float height = 3;
	float width = 0.3;
	FString materialKeyDefault = "white";
	TMap<int, FString> materialsByVertex = {};
	bool doTop = true;
	bool doBottom = false;
	FString topMaterial = "white";
	FString bottomMaterial = "white";
	FString triangleDirection = "clockwise";
};

class ModelBuilding {
public:
	ModelBuilding();
	~ModelBuilding();

	static AActor* Create(TArray<FVector> verticesGround, FModelParams modelParams = FModelParams(),
		FModelCreateParams createParams = FModelCreateParams(),
		FModelBuilding params = FModelBuilding());
};
