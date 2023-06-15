#pragma once

class ModelSide {
public:
	ModelSide();
	~ModelSide();

	static void Left(FString name, FVector size, FVector scale, USceneComponent* parent,
		FString mesh, FString material, FVector offset = FVector(0,0,0));
	static void Right(FString name, FVector size, FVector scale, USceneComponent* parent,
		FString mesh, FString material, FVector offset = FVector(0,0,0));
	static void Back(FString name, FVector size, FVector scale, USceneComponent* parent,
		FString mesh, FString material, FVector offset = FVector(0,0,0));
	static void Front(FString name, FVector size, FVector scale, USceneComponent* parent,
		FString mesh, FString material, FVector offset = FVector(0,0,0));
	static void Bottom(FString name, FVector size, FVector scale, USceneComponent* parent,
		FString mesh, FString material, FVector offset = FVector(0,0,0));
	static void Top(FString name, FVector size, FVector scale, USceneComponent* parent,
		FString mesh, FString material, FVector offset = FVector(0,0,0));
};
