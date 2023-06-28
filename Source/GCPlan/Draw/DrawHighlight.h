#pragma once

#include <mutex>

#include "../Modeling/ModelBase.h"
#include "../ModelingStructsActor.h"

class DrawHighlight {
public:
	DrawHighlight();
	~DrawHighlight();

	DrawHighlight(DrawHighlight &other) = delete;
	void operator=(const DrawHighlight &) = delete;
	static DrawHighlight *GetInstance();

	void CreateOne(FString key, FVector size, FModelParams modelParams,
		FModelCreateParams createParamsIn, TMap<FString, FString> pairs, FString text);
	void DestroyOne(FString key);
	void CleanUp();

private:
	static DrawHighlight *pinstance_;
	static std::mutex mutex_;

	TMap<FString, FString> _activeKeys = {};
};
