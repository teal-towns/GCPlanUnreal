#pragma once

#include "../BuildingStructsActor.h"
#include "../Modeling/ModelBase.h"

struct FTableChairs {
	TMap<FString, TArray<FString>> meshesByTags = {
		{ "table", {} },
		{ "chair", {} },
		{ "flower", {} }
	};
	float tableWidth = 1;
	float tableHeight = 1;
	float chairWidth = 1;
	float edgeSpacing = 1;
	float placeOffset = 2;
	float placeOffsetX = -1;
	float placeOffsetY = -1;
	float placeOffsetMaxXFactor = 0;
	float placeOffsetMaxYFactor = 0;
	FVector offset = FVector(0,0,0);
	FVector rotation = FVector(0,0,0);
	TArray<FString> chairPositions = { "front", "back", "left", "right" };
	float tableItemsProbability = 0.5;
};

class LMTableChairs {
public:
	LMTableChairs();
	~LMTableChairs();

	static TMap<FString, FPolygon> TableWithChairs(FVector size, FModelParams modelParams,
		FModelCreateParams createParamsIn = FModelCreateParams(),
		FTableChairs params = FTableChairs());
	static TMap<FString, FPolygon> TablesAndChairs(FVector size, FModelParams modelParams,
		FModelCreateParams createParamsIn = FModelCreateParams(), FTableChairs params = FTableChairs());
	static TMap<FString, FPolygon> SmallTableChairs(FVector size, FModelParams modelParams,
	FModelCreateParams createParamsIn = FModelCreateParams(), FTableChairs params = FTableChairs());
};
