#pragma once

#include "../BuildingStructsActor.h"
#include "../Modeling/ModelBase.h"

struct FKitchen {
	FVector offset = FVector(0,0,0);
	FVector rotation = FVector(0,0,0);
};

class LMKitchen {
public:
	LMKitchen();
	~LMKitchen();

	static TMap<FString, FPolygon> OfficeWall(FVector size, FModelParams modelParams,
		FModelCreateParams createParamsIn = FModelCreateParams(),
		FKitchen params = FKitchen());
};
