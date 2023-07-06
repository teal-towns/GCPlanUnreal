#pragma once

#include "../BuildingStructsActor.h"
#include "../Modeling/ModelBase.h"

struct FParkingLot {
	FVector offset = FVector(0,0,0);
	FVector rotation = FVector(0,0,0);
	TMap<FString, TArray<FString>> meshesByTags = {
		{ "car", {} },
	};
	float carProbability = 0.75;
};

class LMParkingLot
{
public:
	LMParkingLot();
	~LMParkingLot();

	static TMap<FString, FPolygon> CreateFromInputs();
	static TMap<FString, FPolygon> Create(FVector size, FModelParams modelParams = FModelParams(),
		FModelCreateParams createParamsIn = FModelCreateParams(), FParkingLot params = FParkingLot());
	static TMap<FString, FPolygon> CreateRow(FVector size, FModelParams modelParams = FModelParams(),
		FModelCreateParams createParamsIn = FModelCreateParams(), FParkingLot params = FParkingLot());
};
