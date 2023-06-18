#pragma once

#include "../Modeling/ModelBase.h"
#include "PMBase.h"

class PMCorner {
public:
	PMCorner();
	~PMCorner();

	static AActor* Create(FVector size, TArray<FString> tags,
		FModelCreateParams createParams, FModelParams modelParams = FModelParams(),
		TArray<FPlaneOffsets> xOffsets = {}, TArray<FPlaneOffsets> yOffsets = {});
};
