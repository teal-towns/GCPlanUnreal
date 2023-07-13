#pragma once

#include <mutex>

#include "../Modeling/ModelBase.h"
#include "../Modeling/Common/ModelHighlight.h"
#include "../ModelingStructsActor.h"

struct FDrawHighlight {
	FString key;
	AActor* actor = nullptr;
	float scaleSpeed = 10;
	FVector scale = FVector(1,1,1);
	FVector targetScale = FVector(1,1,1);
	FString materialKey = "white";

	FDrawHighlight() {};
	FDrawHighlight(FString key_, AActor* actor_, float scaleSpeed_ = 10, FVector scale_ = FVector(1,1,1),
		FVector targetScale_ = FVector(1,1,1), FString materialKey_ = "white") {
		key = key_;
		actor = actor_;
		scaleSpeed = scaleSpeed_;
		scale = scale_;
		targetScale = targetScale_;
		materialKey = materialKey_;
	};
};

class DrawHighlight {
public:
	DrawHighlight();
	~DrawHighlight();

	DrawHighlight(DrawHighlight &other) = delete;
	void operator=(const DrawHighlight &) = delete;
	static DrawHighlight *GetInstance();

	void Tick(float DeltaTime);
	void CreateOne(FString key, FVector size, FModelParams modelParams,
		FModelCreateParams createParamsIn, TMap<FString, FString> pairs, FHighlightText params,
		FDrawHighlight drawParams = FDrawHighlight());
	void UpdateOne(FString key, FDrawHighlight drawParams);
	void UpdateTargetScale(FString key, FVector targetScale, float scaleSpeed = -1);
	void DestroyOne(FString key);
	void CleanUp();

private:
	static DrawHighlight *pinstance_;
	static std::mutex mutex_;

	TMap<FString, FDrawHighlight> _items = {};
};
