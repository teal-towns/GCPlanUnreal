#pragma once

#include "Engine/StaticMeshActor.h"

#include "../Modeling/ModelBase.h"

class PMCube {
public:
	PMCube();
	~PMCube();

	static void CreateFromInputs();
	static void Create(FString name, FVector size, TArray<FString> tags = {},
		FModelParams modelParams = FModelParams());
	// static void RoundedCorner(FString name, FVector size, TArray<FString> tags = {});
	static AStaticMeshActor* RoundedTop(FString name, FVector size, TArray<FString> tags,
		float topHeight = 1, FModelParams modelParams = FModelParams());
	static void RoundedTopSide(FVector size, TArray<FString> tags, FModelCreateParams createParams,
		FModelParams modelParams = FModelParams());
};
