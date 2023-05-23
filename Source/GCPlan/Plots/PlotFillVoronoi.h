#pragma once

// USTRUCT()
// struct FPlot {
// 	GENERATED_BODY()

// 	UPROPERTY()
// 	FString _id;
// 	UPROPERTY()
//     FString uName;
//     UPROPERTY()
// 	TMap<FString, float> posCenter;
// 	// TODO - need another struct? Can not do TMap inside TArray??
// 	// UPROPERTY()
//     // TArray<TMap<FString, float>> vertices;
//     TArray<FVector> vertices;
// };

struct FPlot {
	FString _id;
    FString uName;
	// TMap<FString, float> posCenter;
	// TODO - need another struct? Can not do TMap inside TArray??
	// UPROPERTY()
    // TArray<TMap<FString, float>> vertices;
    FVector posCenter;
    TArray<FVector> vertices;
};

class PlotFillVoronoi {
public:
	PlotFillVoronoi();
	~PlotFillVoronoi();

	static std::tuple<TArray<TArray<FVector>>, FVector, TArray<FVector2D>> Fill(TMap<FString, FPlot>, float);
	static TArray<FVector2D> SpawnPoints(TArray<FVector2D>, float, float offsetMaxFactor = 0.1f);
	static std::tuple<bool, TArray<FVector2D>> CheckAdjustVertices(TArray<FVector2D>, TArray<FVector2D>, float minRatioIn = 0.6f);
};
