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

void DrawHighlight::Tick(float DeltaTime) {
	for (auto& Elem : _items) {
		FString key = Elem.Key;
		if (Elem.Value.actor && Elem.Value.scale != Elem.Value.targetScale) {
			if (Elem.Value.scale.X < Elem.Value.targetScale.X) {
				_items[key].scale.X += Elem.Value.scaleSpeed * DeltaTime;
				if (_items[key].scale.X > Elem.Value.targetScale.X) {
					_items[key].scale.X = Elem.Value.targetScale.X;
				}
			} else if (Elem.Value.scale.X > Elem.Value.targetScale.X) {
				_items[key].scale.X -= Elem.Value.scaleSpeed * DeltaTime;
				if (_items[key].scale.X < Elem.Value.targetScale.X) {
					_items[key].scale.X = Elem.Value.targetScale.X;
				}
			}
			if (Elem.Value.scale.Y < Elem.Value.targetScale.Y) {
				_items[key].scale.Y += Elem.Value.scaleSpeed * DeltaTime;
				if (_items[key].scale.Y > Elem.Value.targetScale.Y) {
					_items[key].scale.Y = Elem.Value.targetScale.Y;
				}
			} else if (Elem.Value.scale.Y > Elem.Value.targetScale.Y) {
				_items[key].scale.Y -= Elem.Value.scaleSpeed * DeltaTime;
				if (_items[key].scale.Y < Elem.Value.targetScale.Y) {
					_items[key].scale.Y = Elem.Value.targetScale.Y;
				}
			}
			if (Elem.Value.scale.Z < Elem.Value.targetScale.Z) {
				_items[key].scale.Z += Elem.Value.scaleSpeed * DeltaTime;
				if (_items[key].scale.Z > Elem.Value.targetScale.Z) {
					_items[key].scale.Z = Elem.Value.targetScale.Z;
				}
			} else if (Elem.Value.scale.Z > Elem.Value.targetScale.Z) {
				_items[key].scale.Z -= Elem.Value.scaleSpeed * DeltaTime;
				if (_items[key].scale.Z < Elem.Value.targetScale.Z) {
					_items[key].scale.Z = Elem.Value.targetScale.Z;
				}
			}
			// UE_LOG(LogTemp, Display, TEXT("scale start %s end %s"), *Elem.Value.scale.ToString(), *_items[key].scale.ToString());
			Elem.Value.actor->SetActorScale3D(_items[key].scale);
		}
	}
}

void DrawHighlight::CreateOne(FString key, FVector size, FModelParams modelParams,
	FModelCreateParams createParamsIn, TMap<FString, FString> pairs, FHighlightText params,
	FDrawHighlight drawParams) {
	DestroyOne(key);
	AActor* actor = ModelHighlight::Create(size, modelParams, createParamsIn, pairs, params);
	if (drawParams.scale != drawParams.targetScale && size != drawParams.scale) {
		actor->SetActorScale3D(drawParams.scale);
	}
	// UE_LOG(LogTemp, Display, TEXT("CreateOne %s target %s"), *drawParams.scale.ToString(), *drawParams.targetScale.ToString());
	_items.Add({ key, FDrawHighlight(key, actor, drawParams.scaleSpeed, drawParams.scale,
		drawParams.targetScale, drawParams.materialKey) });
}

void DrawHighlight::UpdateOne(FString key, FDrawHighlight drawParams) {
	if (_items.Contains(key)) {
		drawParams.actor = _items[key].actor;
		_items[key] = drawParams;
	}
}

void DrawHighlight::UpdateTargetScale(FString key, FVector targetScale, float scaleSpeed) {
	if (_items.Contains(key)) {
		_items[key].targetScale = targetScale;
		if (scaleSpeed > 0) {
			_items[key].scaleSpeed = scaleSpeed;
		}
	}
}

void DrawHighlight::DestroyOne(FString key) {
	if (_items.Contains(key)) {
		ModelBase* modelBase = ModelBase::GetInstance();
		modelBase->DestroyActorByKey(_items[key].actor->GetName());
		_items.Remove(key);
	}
}

void DrawHighlight::CleanUp() {
	ModelBase* modelBase = ModelBase::GetInstance();
	for (auto& Elem : _items) {
		modelBase->DestroyActorByKey(Elem.Value.actor->GetName());
	}
	_items.Empty();
}
