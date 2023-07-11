#pragma once

#include "ProceduralMeshComponent.h"

#include "PMBase.h"
#include "../Modeling/ModelBase.h"

struct FPlanePolygon {
	FString triangleDirection = "clockwise";
	FString name = "";
};

class PMPlanePolygon {
public:
	PMPlanePolygon();
	~PMPlanePolygon();

	static AActor* Create(TArray<FVector> vertices,
		FModelCreateParams createParams = FModelCreateParams(),
		FModelParams modelParams = FModelParams(), FPlanePolygon params = FPlanePolygon());
};
