#pragma once

#include "../DataStructsActor.h"

struct FPlaceParams {
	float scaleMin = 0.75;
	float scaleMax = 2;
	float rotMinX = 0;
	float rotMaxX = 0;
	float rotMinY = 0;
	float rotMaxY = 0;
	float rotMinZ = 0;
	float rotMaxZ = 360;

	int closedLoop = 0;
	bool snapToGround = false;
	float radius = -1;
	FString shape = "square";
	float offsetAverage = 10;
	float offsetMaxFactorX = 0.5;
	float offsetMaxFactorY = 0.5;
	float offsetMaxFactorZ = 0.5;
	float maxZOffset = 0;
	float minZOffset = 0;
	FString spreadPattern = "grid";
	int maxCount = -1;
	int maxCountMin = -1;
	int maxCountMax = -1;

	TArray<TArray<FVector>> skipPolygons = {};
	TArray<TArray<FVector2D>> skipPolygons2D = {};

	float width = 10;
	float spacing = 10;
	float spacingFactor = 0.25;
	float spacingCrossAxis = 3;
	float spacingFactorCrossAxis = 0.25;

	FPlaceParams() {};
};

class LayoutPlace {
public:
	LayoutPlace();
	~LayoutPlace();

	static std::tuple<FString, FMeshTransform> PlaceMesh(FVector pos, TArray<FString> meshNames,
		FPlaceParams params);
};