#pragma once

#include "ProceduralMeshComponent.h"

#include "PMBase.h"
#include "../Modeling/ModelBase.h"

class PMPlane {
public:
	PMPlane();
	~PMPlane();

	// static UStaticMesh* CreateFromInputs();
	static AActor* Create(FString name, FVector size, TArray<FPlaneOffsets> xOffsets,
		TArray<FPlaneOffsets> yOffsets, FString plane = "xy",
		FModelCreateParams createParams = FModelCreateParams(),
		FModelParams modelParams = FModelParams());
	// static std::tuple<TMap<FString, FString>, TArray<FPlaneOffsets>, TArray<FPlaneOffsets>> GetInputs(
	// 	TArray<FPlaneOffsets> xOffsets = {}, TArray<FPlaneOffsets> yOffsets = {},
	// 	TMap<FString, FString> offsetsCombine = {});
	static TArray<FVector> VerticesFromOffsets(FVector size, TArray<FPlaneOffsets> offsets,
		FVector offset = FVector(0,0,0));
	// static TArray<FVector> ComputeVertices(FString name, FVector size, TArray<FPlaneOffsets> xOffsets,
	// 	TArray<FPlaneOffsets> yOffsets, FString plane = "xy",
	// 	TArray<FString> tags = {}, FModelCreateParams createParams = FModelCreateParams(),
	// 	FModelParams modelParams = FModelParams());
	static FVector ComputeVertex(int xx, int yy, FVector size, TArray<FPlaneOffsets> xOffsets,
		TArray<FPlaneOffsets> yOffsets, FString plane,
		FModelCreateParams createParams, float xStart, float yStart, float zStart);
	static FVector MapXYZ(FVector xyz, FString plane);
	// static std::tuple<TArray<FPlaneOffsets>, TArray<FPlaneOffsets>> RoundedOffsets();
	// static TArray<FPlaneOffsets> BorderRadiusTop(float radiusRatio, float startRatio,
	// 	FString anchor = "start");
};
