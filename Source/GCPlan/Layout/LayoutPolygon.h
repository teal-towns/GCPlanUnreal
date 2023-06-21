#pragma once

#include <mutex>

#include "LayoutPlace.h"
#include "../DataStructsActor.h"

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

	bool PlaceInPolygon(TArray<FVector> vertices, TArray<FString> meshNames,
		FPlaceParams params = FPlaceParams(), FVector posCenter = FVector(0,0,0));
	std::tuple<FString, FMeshTransform> CheckAddObj(FVector pos,
		TArray<FString> meshNames, TArray<FVector2D> vertices2D, FVector posCenter,
		FPlaceParams params = FPlaceParams());
	bool InPolygon(FVector pos, TArray<FVector2D> vertices2D, FVector posCenter,
		float radius, FString shape);
};
