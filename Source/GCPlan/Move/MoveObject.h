#pragma once

#include <mutex>

struct FMoving {
	FVector start;
	FVector end;
	FVector current;
	AActor* actor;
	float speed;

	FMoving() {};
	FMoving(FVector start_, FVector end_, FVector current_, AActor* actor_, float speed_) {
		start = start_;
		end = end_;
		current = current_;
		actor = actor_;
		speed = speed_;
	};
};

class MoveObject {
public:
	MoveObject();
	~MoveObject();

	// Singletons should not be cloneable or assignable.
	MoveObject(MoveObject &other) = delete;
	void operator=(const MoveObject &) = delete;

	static MoveObject *GetInstance();

	void Tick(float DeltaTime);
	void CleanUp();
	void Remove(FString name);
	void Move(AActor* actor, FVector end, float speed);

private:
	static MoveObject *pinstance_;
	static std::mutex mutex_;

	TMap<FString, FMoving> _movingActors = {};
};
