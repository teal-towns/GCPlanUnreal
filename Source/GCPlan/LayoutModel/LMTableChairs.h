#pragma once

#include "../BuildingStructsActor.h"
#include "../Modeling/ModelBase.h"

struct FTableChairs {
	TArray<FString> tableMeshes = {};
	TArray<FString> chairMeshes = {};
	float chairWidth = 1;
	float edgeSpacing = 1;
	FVector offset = FVector(0,0,0);
};

class LMTableChairs {
public:
	LMTableChairs();
	~LMTableChairs();

	static TMap<FString, FPolygon> TableWithChairs(FVector size, FModelParams modelParams,
		FModelCreateParams createParamsIn = FModelCreateParams(),
		FTableChairs params = FTableChairs());
};
