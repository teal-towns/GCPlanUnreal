#pragma once

#include <mutex>

#include "LayoutPlace.h"
#include "../DataStructsActor.h"

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

	static TMap<FString, FMeshTransform> PlaceOnLine(TArray<FVector> vertices, TArray<FString> meshNames,
		FPlaceParams params = FPlaceParams());
	static bool PlaceOnLineSides(TArray<FVector> vertices, float width, TArray<FString> meshNames,
		FPlaceParams params);
};
