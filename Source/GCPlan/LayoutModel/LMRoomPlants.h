#pragma once

#include "../BuildingStructsActor.h"
#include "../Modeling/ModelBase.h"

struct FWallPlanterBox {
	FVector offset = FVector(0,0,0);
	FVector rotation = FVector(0,0,0);
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

struct FPlanterBox {
	FVector offset = FVector(0,0,0);
	FVector rotation = FVector(0,0,0);
	TMap<FString, TArray<FString>> meshesByTags = {
		{ "indoorBush", {} },
		{ "indoorBushTall", {} }
	};
	// float plantWidth = 0.6;
	float planterHeight = 1;
	FString meshRule = "planterBox";
};

struct FHangingPlants {
	FVector offset = FVector(0,0,0);
	FVector rotation = FVector(0,0,0);
	TMap<FString, TArray<FString>> meshesByTags = {
		{ "hangingPlant", {} },
		{ "flowerPot", {} }
	};
	float placeOffset = 1;
	float placeOffsetMaxXFactor = 0;
	float placeOffsetMaxYFactor = 0;
	FVector cordScale = FVector(0.03, 0.03, 1.5);
	FVector potScale = FVector(0.3, 0.3, 0.25);
};

class LMRoomPlants {
public:
	LMRoomPlants();
	~LMRoomPlants();

	static TMap<FString, FPolygon> WallPlanterBox(FVector size, FModelParams modelParams,
		FModelCreateParams createParamsIn = FModelCreateParams(),
		FWallPlanterBox params = FWallPlanterBox());
	static TMap<FString, FPolygon> MultiPlantsBox(FVector size, FModelParams modelParams,
		FModelCreateParams createParamsIn = FModelCreateParams(),
		FWallPlanterBox params = FWallPlanterBox());
	static TMap<FString, FPolygon> PlanterBox(FVector size, FModelParams modelParams = FModelParams(),
		FModelCreateParams createParamsIn = FModelCreateParams(), FPlanterBox params = FPlanterBox());
	static TMap<FString, FPolygon> WallPlants(FVector size, FModelParams modelParams = FModelParams(),
		FModelCreateParams createParamsIn = FModelCreateParams(),
		FWallPlants params = FWallPlants());
	static TMap<FString, FPolygon> HangingPlants(FVector size, FModelParams modelParams,
		FModelCreateParams createParamsIn = FModelCreateParams(), FHangingPlants params = FHangingPlants());
};
