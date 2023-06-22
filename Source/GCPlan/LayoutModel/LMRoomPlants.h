#pragma once

#include "../BuildingStructsActor.h"
#include "../Modeling/ModelBase.h"

struct FPlanterBox {
	float width = 0.6;
	float height = 0.3;
	float wallOffsetWidth = 1;
	float wallOffsetLength = 2;
	FString pairsStringPlants = "";
	TArray<FString> walls = { "front", "back", "left", "right" };
};

struct FWallPlants {
	float zOffset = 0.5;
	float sideOffset = 1;
	float width = 0.05;
	FString pairsStringPlants = "";
	TArray<FString> walls = { "front", "back", "left", "right" };
};

class LMRoomPlants {
public:
	LMRoomPlants();
	~LMRoomPlants();

	static TMap<FString, FPolygon> WallPlanterBox(FVector size, FModelParams modelParams,
		FModelCreateParams createParamsIn = FModelCreateParams(),
		FPlanterBox params = FPlanterBox());
	static TMap<FString, FPolygon> WallPlants(FVector size, FModelParams modelParams = FModelParams(),
		FModelCreateParams createParamsIn = FModelCreateParams(),
		FWallPlants params = FWallPlants());
};
