#pragma once
#include "ProceduralMeshComponent.h"

class PMPrism
{
public:
	PMPrism();
	~PMPrism();

	static UStaticMesh *CreateFromInputs();
	static std::tuple<TArray<FVector>, TArray<int32>, TArray<FVector>, TArray<FProcMeshTangent>, TArray<FVector2D>> InitializeMesh(int32 sidesSegmentCount, bool closeTop, bool closeBottom, bool doubleSided);
	static UStaticMesh *Create(FString name, int32 sidesSegmentCount = 4, float height = 1.0, float width = 1.0,
							   float topOffsetWidth = 0.0, bool closeTop = true, bool closeBottom = true, bool destroyActor = false);
};
