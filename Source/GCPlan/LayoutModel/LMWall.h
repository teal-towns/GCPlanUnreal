#pragma once

#include "../BuildingStructsActor.h"
#include "../Modeling/ModelBase.h"

struct FLayoutWall {
	TMap<FString, TArray<FString>> meshesByTags = {
		{ "door", {} }
	};
	float doorLeft = 1;
	FString side = "back";
	FVector offset = FVector(0,0,0);
	FVector rotation = FVector(0,0,0);
	FVector wallRotation = FVector(0,0,0);
	FVector doorRotation = FVector(0,0,0);
	FString materialKey = "white";
};

class LMWall {
public:
	LMWall();
	~LMWall();

	static TMap<FString, FPolygon> Wall(FVector size, FModelParams modelParams,
		FModelCreateParams createParamsIn = FModelCreateParams(),
		FLayoutWall params = FLayoutWall());
};
