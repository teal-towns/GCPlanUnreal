#pragma once

#include "../BuildingStructsActor.h"
#include "../Modeling/ModelBase.h"

struct FLayoutLights {
	FVector offset = FVector(0,0,0);
	FVector rotation = FVector(0,0,0);
	float placeOffset = 3;
	float placeOffsetMaxXFactor = 0;
	float placeOffsetMaxYFactor = 0;
	FVector sizeBuffer = FVector(-3,-3,0);
	FVector lightScale = FVector(0.1, 0.1, 0.1);
};

class LMLights {
public:
	LMLights();
	~LMLights();

	static TMap<FString, FPolygon> Ceiling(FVector size, FModelParams modelParams,
		FModelCreateParams createParamsIn = FModelCreateParams(),
		FLayoutLights params = FLayoutLights());
};
