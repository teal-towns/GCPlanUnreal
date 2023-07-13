#pragma once

#include <mutex>

struct FMovePolyLine {
	FString key;
	TArray<FVector> vertices;
	AActor* actorTemplate;
	float speed = 10;
	FVector scale = FVector(0, 1, 1);
	int startTickDelay = 0;
	FString materialKey = "white";
	int currentIndex = 0;
	float currentScaleX = 0;
	float currentMaxScaleX = 1;
	float maxScale = -1;
	float scaleSpeed = 10;
	bool movingActive = true;
	bool scalingActive = false;

	FMovePolyLine() {};
	FMovePolyLine(FString key_, TArray<FVector> vertices_, AActor* actorTemplate_, float speed_,
		FVector scale_ = FVector(0,1,1), int startTickDelay_ = 0, FString materialKey_ = "white",
		float currentMaxScaleX_ = 1) {
		key = key_;
		vertices = vertices_;
		actorTemplate = actorTemplate_;
		speed = speed_;
		scale = scale_;
		startTickDelay = startTickDelay_;
		materialKey = materialKey_;
		currentMaxScaleX = currentMaxScaleX_;
	};
};

class MovePolyLine {
public:
	MovePolyLine();
	~MovePolyLine();

	// Singletons should not be cloneable or assignable.
	MovePolyLine(MovePolyLine &other) = delete;
	void operator=(const MovePolyLine &) = delete;

	static MovePolyLine *GetInstance();

	void Tick(float DeltaTime);
	void SetScale(FString key);
	void CleanUp(bool destroy = true);
	void Remove(FString name);
	void SetWorld(UWorld* world);
	void ReScale(float maxScale, float scaleSpeed);
	float GetLineMaxXScale(FVector start, FVector end);
	FString FormName(FString key, int ii);
	void CreateActors(FString key, TArray<FVector> vertices, AActor* actorTemplate,
		FString materialKey = "white", FVector scaleBase = FVector(0,1,1));
	void Move(FString key, TArray<FVector> vertices, AActor* actorTemplate,
		FString materialKey = "white", float speed = 10, FVector scale = FVector(0,1,1),
		int startTickDelay = 0);
	void DrawFull(FString key, TArray<FVector> vertices, AActor* actorTemplate,
		FString materialKey = "white");

private:
	static MovePolyLine *pinstance_;
	static std::mutex mutex_;

	TMap<FString, FMovePolyLine> _movingLines = {};
	UWorld* _world = nullptr;
public:
	TMap<FString, AActor*> _actors = {};
};
