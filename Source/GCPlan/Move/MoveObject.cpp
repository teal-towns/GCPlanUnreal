#include "MoveObject.h"

#include "Components/SceneComponent.h"
#include "Math/UnrealMathUtility.h"

#include "../Common/UnrealGlobal.h"

MoveObject* MoveObject::pinstance_{nullptr};
std::mutex MoveObject::mutex_;

MoveObject::MoveObject() {
}

MoveObject::~MoveObject() {
}

MoveObject *MoveObject::GetInstance() {
	std::lock_guard<std::mutex> lock(mutex_);
	if (pinstance_ == nullptr) {
		pinstance_ = new MoveObject();
	}
	return pinstance_;
}

void MoveObject::Tick(float DeltaTime) {
	FString name;
	TArray<FString> removeKeys = {};
	AActor* actor;
	for (auto& Elem : _movingActors) {
		name = Elem.Key;
		// FVector current = _movingActors[name].current;
		// current.X += _movingActors[name].speed * 100 * DeltaTime;
		// // current.Y += _movingActors[name].speed * DeltaTime;
		// // current.Z += _movingActors[name].speed * DeltaTime;
		// _movingActors[name].current = current;
		// // Already in unreal scale.
		// // TODO - not working; need to move component.. why? Tried setting to root component but it did not work or something?
		// // _movingActors[name].actor->SetActorLocation(_movingActors[name].current);
		// actor = _movingActors[name].actor;
		// USceneComponent* component = actor->FindComponentByClass<USceneComponent>();
		// component->SetRelativeLocation(_movingActors[name].current);

		// TODO - not working; need to move component.. why? Tried setting to root component but it did not work or something?
		// _movingActors[name].current = _movingActors[name].actor->GetActorLocation();
		// _movingActors[name].actor->SetActorLocation(FMath::VInterpTo(
		// 	_movingActors[name].current, _movingActors[name].end,
		// 	DeltaTime, _movingActors[name].speed));
		actor = _movingActors[name].actor;
		USceneComponent* component = actor->FindComponentByClass<USceneComponent>();
		_movingActors[name].current = component->GetComponentLocation();
		component->SetRelativeLocation(FMath::VInterpTo(
			_movingActors[name].current, _movingActors[name].end,
			DeltaTime, _movingActors[name].speed));
		// UE_LOG(LogTemp, Display, TEXT("MoveObject.Tick num %d name %s current %s end %s"), _movingActors.Num(), *name, *_movingActors[name].current.ToString(), *_movingActors[name].end.ToString());
		// See if done.
		if ((_movingActors[name].current - _movingActors[name].end).Size() < 10) {
			UE_LOG(LogTemp, Display, TEXT("MoveObject near end, removing %s current %s"), *name, *_movingActors[name].current.ToString());
			removeKeys.Add(name);
		}
	}
	for (int ii = 0; ii < removeKeys.Num(); ii++) {
		Remove(removeKeys[ii]);
	}
}

void MoveObject::CleanUp() {
	_movingActors.Empty();
}

void MoveObject::Remove(FString name) {
	if (_movingActors.Contains(name)) {
		_movingActors.Remove(name);
	}
}

void MoveObject::Move(AActor* actor, FVector end, float speed) {
	FString name = actor->GetName();
	UE_LOG(LogTemp, Display, TEXT("MoveObject.Move %s"), *name);
	UnrealGlobal* unrealGlobal = UnrealGlobal::GetInstance();
	// Leave in Unreal scale since will be moving every tick.
	FVector start = actor->GetActorLocation();
	// Convert to Unreal scale.
	end = end * unrealGlobal->GetScale();
	FMoving moving = FMoving(start, end, start, actor, speed);
	if (_movingActors.Contains(name)) {
		_movingActors[name] = moving;
	} else {
		_movingActors.Add(name, moving);
	}
}
