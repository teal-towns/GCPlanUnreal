#pragma once

#include "../ModelBase.h"

struct FHighlightText {
	FString text = "";
	FString subText = "";
	FVector subTextScale = FVector(0.5,0.5,0.5);
	FVector textOffset = FVector(0, 1, 2.5);
	FVector textRotation = FVector(0,0,0);
	FVector subTextOffset = FVector(0,0,-2);
	FVector dotScale = FVector(2,2,2);
	float lineFactor = 1;
	FString dotMeshKey = "sphere";
	FVector dotRotation = FVector(0,0,0);
	FString textAlignment = "left";
};

class ModelHighlight {
public:
	ModelHighlight();
	~ModelHighlight();

	static AActor* Create(FVector size, FModelParams modelParams = FModelParams(),
		FModelCreateParams createParams = FModelCreateParams(), TMap<FString, FString> pairs = {},
		FHighlightText params = FHighlightText());
};
