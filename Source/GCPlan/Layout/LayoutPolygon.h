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

	static TMap<FString, FMeshTransform> PlaceInPolygon(TArray<FVector> vertices, TArray<FString> meshNames,
		FPlaceParams params = FPlaceParams(), FVector posCenter = FVector(0,0,0));
	static std::tuple<FString, FMeshTransform> CheckAddObj(FVector pos,
		TArray<FString> meshNames, TArray<FVector2D> vertices2D, FVector posCenter,
		TArray<FVector> minMaxPoints, FPlaceParams params = FPlaceParams());
	static bool InPolygon(FVector pos, FVector2D pos2D, TArray<FVector2D> vertices2D, FVector posCenter,
		float radius, FString shape);
};
