#pragma once

#include "../BuildingStructsActor.h"
#include "../Modeling/ModelBase.h"

struct FDesks {
	TMap<FString, TArray<FString>> meshesByTags = {
		{ "indoorBush", {} },
		{ "laptop", {} },
		{ "officeChair", {} },
		{ "deskAccessory", {} },
		{ "pcMonitor", {} },
		{ "pcKeyboard", {} },
		{ "pcMouse", {} },
		{ "pcTablet", {} }
	};
	FVector offset = FVector(0,0,0);
	FVector rotation = FVector(0,0,0);
	float plantWidth = 0.3;
	float planterHeight = 1.3;
	float chairWidth = 1;
	float deskWidth = 1;
	float deskHeightMin = 0.7;
	float deskHeightMax = 1.5;
	float deskItemsProbability = 0.5;
	int plantsPerDeskRow = 3;
};

class LMDesks {
public:
	LMDesks();
	~LMDesks();

	static TMap<FString, FPolygon> TwoDesksRow(FVector size, FModelParams modelParams,
		FModelCreateParams createParamsIn = FModelCreateParams(),
		FDesks params = FDesks());
	static TMap<FString, FPolygon> Desk(FVector size, FModelParams modelParams,
		FModelCreateParams createParamsIn = FModelCreateParams(), FDesks params = FDesks());
};
