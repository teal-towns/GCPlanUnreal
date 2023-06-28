#include "DrawHighlight.h"

#include "../Modeling/ModelBase.h"
#include "../Modeling/Common/ModelHighlight.h"
#include "../ModelingStructsActor.h"

DrawHighlight* DrawHighlight::pinstance_{nullptr};
std::mutex DrawHighlight::mutex_;

DrawHighlight::DrawHighlight() {
}

DrawHighlight::~DrawHighlight() {
}

DrawHighlight *DrawHighlight::GetInstance() {
	std::lock_guard<std::mutex> lock(mutex_);
	if (pinstance_ == nullptr) {
		pinstance_ = new DrawHighlight();
	}
	return pinstance_;
}

void DrawHighlight::CreateOne(FString key, FVector size, FModelParams modelParams,
	FModelCreateParams createParamsIn, TMap<FString, FString> pairs, FString text) {
	DestroyOne(key);
	AActor* actor = ModelHighlight::Create(size, modelParams, createParamsIn, pairs, text);
	_activeKeys.Add({ key, actor->GetName() });
}

void DrawHighlight::DestroyOne(FString key) {
	if (_activeKeys.Contains(key)) {
		ModelBase* modelBase = ModelBase::GetInstance();
		modelBase->DestroyActorByKey(_activeKeys[key]);
		_activeKeys.Remove(key);
	}
}

void DrawHighlight::CleanUp() {
	ModelBase* modelBase = ModelBase::GetInstance();
	for (auto& Elem : _activeKeys) {
		modelBase->DestroyActorByKey(Elem.Value);
	}
	_activeKeys.Empty();
}
