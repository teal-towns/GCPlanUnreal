#pragma once

class ModelLeg {
public:
	ModelLeg();
	~ModelLeg();

	static void FrontRight(FString name, FVector size, FVector scale, float buffer,
		USceneComponent* parent, FString mesh, FString material, FVector offset = FVector(0,0,0));
	static void BackRight(FString name, FVector size, FVector scale, float buffer,
		USceneComponent* parent, FString mesh, FString material, FVector offset = FVector(0,0,0));
	static void FrontLeft(FString name, FVector size, FVector scale, float buffer,
		USceneComponent* parent, FString mesh, FString material, FVector offset = FVector(0,0,0));
	static void BackLeft(FString name, FVector size, FVector scale, float buffer,
		USceneComponent* parent, FString mesh, FString material, FVector offset = FVector(0,0,0));
};
