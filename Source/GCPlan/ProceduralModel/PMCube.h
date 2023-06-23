#pragma once

#include "Engine/StaticMeshActor.h"

#include "../Modeling/ModelBase.h"

class PMCube {
public:
	PMCube();
	~PMCube();

	static void CreateFromInputs();
	static void Create(FString name, FVector size, FModelParams modelParams = FModelParams());
	static AActor* Plane(FString name, FVector size, FModelParams modelParams);
	static AStaticMeshActor* RoundedTop(FString name, FVector size,
		float topHeight = 1, FModelParams modelParams = FModelParams());
	static void RoundedTopSide(FVector size, FModelCreateParams createParams,
		FModelParams modelParams = FModelParams());
};
