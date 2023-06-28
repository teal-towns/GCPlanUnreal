#include "MoveObject.h"

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
	UE_LOG(LogTemp, Display, TEXT("MoveObject.Tick num %d"), _movingActors.Num());
	AActor* actor;
	for (auto& Elem : _movingActors) {
		name = Elem.Key;
		// FVector current = _movingActors[name].current;
		// current.X += _movingActors[name].speed * DeltaTime;
		// current.Y += _movingActors[name].speed * DeltaTime;
		// current.Z += _movingActors[name].speed * DeltaTime;
		// _movingActors[name].current = current;
		// // Already in unreal scale.
		// _movingActors[name].actor->SetActorLocation(_movingActors[name].current);
		_movingActors[name].current = _movingActors[name].actor->GetActorLocation();
		_movingActors[name].actor->SetActorLocation(FMath::VInterpTo(
			_movingActors[name].current, _movingActors[name].end,
			DeltaTime, _movingActors[name].speed));
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

void MoveObject::Remove(FString name) {
	if (_movingActors.Contains(name)) {
		_movingActors.Remove(name);
	}
}

void MoveObject::Move(AActor* actor, FVector end, float speed) {
	FString name = actor->GetName();
	UE_LOG(LogTemp, Display, TEXT("MoveObject.Move %s"), *name);
	UnrealGlobal* unrealGlobal = UnrealGlobal::GetInstance();
	// Convert to Unreal scale since will be moving every tick.
	FVector start = actor->GetActorLocation() * unrealGlobal->GetScale();
	end = end * unrealGlobal->GetScale();
	FMoving moving = FMoving(start, end, start, actor, speed);
	if (_movingActors.Contains(name)) {
		_movingActors[name] = moving;
	} else {
		_movingActors.Add(name, moving);
	}
}
