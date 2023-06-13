#pragma once
#include "ProceduralMeshComponent.h"

class PMPrism
{
public:
	PMPrism();
	~PMPrism();

	static UStaticMesh *CreateFromInputs();
	static std::tuple<TArray<FVector>, TArray<int32>, TArray<FVector>, TArray<FProcMeshTangent>, TArray<FVector2D>> InitializeMesh(int32 sidesSegmentCount, bool capTop, bool capBottom, bool doubleSided);
	static UStaticMesh *Create(FString name, TArray<FString> tags = {}, int32 sidesSegmentCount = 4, float height = 1.0, float width = 1.0,
							   float topOffsetWidth = 0.0, bool capTop = true, bool capBottom = true, bool destroyActor = false);
};
