#pragma once

#include "../BuildingStructsActor.h"
#include "../Modeling/ModelBase.h"

class LMLobby {
public:
	LMLobby();
	~LMLobby();

	static TMap<FString, FPolygon> CreateFromInputs();
	static TMap<FString, FPolygon> Create(FVector size, FModelParams modelParams = FModelParams(),
		FModelCreateParams createParamsIn = FModelCreateParams());
	static TMap<FString, FPolygon> TwoTables(FVector size, FModelParams modelParams,
		FModelCreateParams createParamsIn);
	static TMap<FString, FPolygon> CouchesCoffeeTables(FVector size, FModelParams modelParams,
		FModelCreateParams createParamsIn = FModelCreateParams());
};
