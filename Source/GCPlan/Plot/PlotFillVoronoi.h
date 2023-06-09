#pragma once

#include "../BuildingStructsActor.h"

class PlotFillVoronoi {
public:
	PlotFillVoronoi();
	~PlotFillVoronoi();

	static std::tuple<TArray<TArray<FVector>>, FVector, TArray<FVector2D>> Fill(TMap<FString, FPlot>, float);
	static TArray<TArray<FVector>> SpawnSpaces(TArray<FVector2D> boundsRect,
		float averageDistance, float offsetMaxFactor = 0.1f);
	static TArray<FVector2D> SpawnPoints(TArray<FVector2D>, float, float offsetMaxFactor = 0.1f);
	static std::tuple<bool, TArray<FVector2D>> CheckAdjustVertices(TArray<FVector2D>, TArray<FVector2D>, float minRatioIn = 0.6f);
	static TArray<TArray<FVector>> BufferAndRemoveVertices(TArray<TArray<FVector>> spacesVertices,
	float verticesBuffer, float minRadiusSkip);
	static std::tuple<bool, FString> IsValid(TArray<FVector> pathVertices, float minRadiusSkip = 20,
		int minVerticesSkip = 5);
};
