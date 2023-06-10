#pragma once

#include <mutex>

#include "../DataStructsActor.h"

struct FPlaceParams {
	bool snapToGround = false;
	float radius = -1;
	FString shape = "square";
	float offsetAverage = 10;
	float offsetMaxFactorX = 0.5;
	float offsetMaxFactorY = 0.5;
	float offsetMaxFactorZ = 0.5;
	float maxZOffset = 0;
	float minZOffset = 0;
	float spacing = 1;
	FString spreadPattern = "grid";
	int maxCount = -1;
	int maxCountMin = -1;
	int maxCountMax = -1;
	float scaleMin = 0.75;
	float scaleMax = 2;
	float rotMinX = 0;
	float rotMaxX = 0;
	float rotMinY = 0;
	float rotMaxY = 0;
	float rotMinZ = 0;
	float rotMaxZ = 360;
	TArray<TArray<FVector>> skipPolygons = {};
	TArray<TArray<FVector2D>> skipPolygons2D = {};

	FPlaceParams() {};
};

class LayoutPolygon {
private:
	static LayoutPolygon *pinstance_;
	static std::mutex mutex_;

public:
	LayoutPolygon();
	~LayoutPolygon();

	// Singletons should not be cloneable or assignable.
	LayoutPolygon(LayoutPolygon &other) = delete;
	void operator=(const LayoutPolygon &) = delete;
	static LayoutPolygon *GetInstance();

	// void PlaceInPolygon(FVector vertices, TArray<FString> meshNames, 
	// 	FVector posCenter = FVector(0,0,0), bool snapToGround = false, float radius = -1, FString shape = "square",
	// 	float offsetAverage = 10, float offsetMaxFactorX = 0.25, float offsetMaxFactorY = 0.25,
	// 	float offsetMaxFactorZ = 0.25, float maxZOffset = 0, float minZOffset = 0,
	// 	float spacing = 1, FString spreadPattern = "grid", int maxCount = -1,
	// 	int maxCountMin = -1, int maxCountMax = -1,
	// 	float scaleMin = 0.75, float scaleMax = 2,
	// 	float rotMinX = 0, float rotMaxX = 0,
	// 	float rotMinY = 0, float rotMaxY = 0, float rotMinZ = 0, float rotMaxZ = 360);
	bool PlaceInPolygon(TArray<FVector> vertices, TArray<FString> meshNames,
		FVector posCenter = FVector(0,0,0), FPlaceParams params = FPlaceParams());
	// std::tuple<FString, FMeshTransform> CheckAddObj(FVector pos,
	// 	TArray<FString> meshNames, TArray<FVector2D> vertices2D, FVector posCenter, float radius,
	// 	FString shape, float scaleMin, float scaleMax, float rotMinX, float rotMaxX,
	// 	float rotMinY, float rotMaxY, float rotMinZ, float rotMaxZ, bool snapToGround);
	std::tuple<FString, FMeshTransform> CheckAddObj(FVector pos,
		TArray<FString> meshNames, TArray<FVector2D> vertices2D, FVector posCenter,
		FPlaceParams params = FPlaceParams());
	bool InPolygon(FVector pos, TArray<FVector2D> vertices2D, FVector posCenter,
		float radius, FString shape);
};
