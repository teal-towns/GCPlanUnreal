#pragma once

#include "../../BuildingStructsActor.h"
#include "../../Modeling/ModelBase.h"

struct FTrain {
	FVector offset = FVector(0,0,0);
	FVector rotation = FVector(0,0,0);
	int traincars = 8;
};

class LMTrain {
public:
	LMTrain();
	~LMTrain();

	static TMap<FString, FPolygon> Create(FVector size, FModelParams modelParams = FModelParams(),
		FModelCreateParams createParamsIn = FModelCreateParams(), FTrain params = FTrain());
};
