#pragma once

#include "PMBase.h"
#include "../Modeling/ModelBase.h"

class PMPlaneU {
public:
	PMPlaneU();
	~PMPlaneU();

	static AActor* Create(FString name, FVector size,
		FModelCreateParams createParams = FModelCreateParams(), FModelParams modelParams = FModelParams(),
		FString direction = "z", TArray<FPlaneOffsets> xOffsets = {}, TArray<FPlaneOffsets> yOffsets = {});
	static TArray<FPlaneOffsets> GetOffsetsStart(FString direction = "z", float ratioFactor = 1);
	static TArray<FPlaneOffsets> GetOffsetsEnd(FString direction = "z", float ratioFactor = 1);
	static AActor* Shape(FString name, FVector size, FModelCreateParams createParams, FModelParams modelParams,
		float thickness);
};
