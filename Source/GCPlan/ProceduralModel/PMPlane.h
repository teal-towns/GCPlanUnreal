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
		TArray<FPlaneOffsets> yOffsets, FString plane = "xy", int triangleSide = 1,
		TArray<FString> tags = {}, FModelCreateParams createParams = FModelCreateParams(),
		FModelParams modelParams = FModelParams(), TMap<FString, FString> offsetsCombine = {});
	static FVector MapXYZ(FVector xyz, FString plane);
	// static std::tuple<TArray<FPlaneOffsets>, TArray<FPlaneOffsets>> RoundedOffsets();
	// static TArray<FPlaneOffsets> BorderRadiusTop(float radiusRatio, float startRatio,
	// 	FString anchor = "start");
};
