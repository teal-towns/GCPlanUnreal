#pragma once

#include <mutex>

#include "LayoutPlace.h"
#include "../DataStructsActor.h"

// struct FPolyLinePlaceParams {
// 	bool snapToGround = false;
// 	float width = 10;
// 	float spacing = 10;
// 	float spacingFactor = 0.25;
// 	float spacingCrossAxis = 5;
// 	float spacingFactorCrossAxis = 0.25;
// 	float scaleMin = 0.75;
// 	float scaleMax = 2;
// 	float rotMinX = 0;
// 	float rotMaxX = 0;
// 	float rotMinY = 0;
// 	float rotMaxY = 0;
// 	float rotMinZ = 0;
// 	float rotMaxZ = 360;
// 	int closedLoop = 0;

// 	FPolyLinePlaceParams() {};
// };

class LayoutPolyLine {
private:
	static LayoutPolyLine *pinstance_;
	static std::mutex mutex_;

public:
	LayoutPolyLine();
	~LayoutPolyLine();

	// Singletons should not be cloneable or assignable.
	LayoutPolyLine(LayoutPolyLine &other) = delete;
	void operator=(const LayoutPolyLine &) = delete;
	static LayoutPolyLine *GetInstance();

	static bool PlaceOnLine(TArray<FVector> vertices, TArray<FString> meshNames,
		FPlaceParams params = FPlaceParams());
	static bool PlaceOnLineSides(TArray<FVector> vertices, float width, TArray<FString> meshNames,
		FPlaceParams params);
};
