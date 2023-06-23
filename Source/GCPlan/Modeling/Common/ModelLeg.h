#pragma once

#include "../ModelBase.h"

class ModelLeg {
public:
	ModelLeg();
	~ModelLeg();

	static void FrontRight(FString name, FVector size, FVector scale, float buffer,
		FModelParams modelParams, FVector offset = FVector(0,0,0));
	static void BackRight(FString name, FVector size, FVector scale, float buffer,
		FModelParams modelParams, FVector offset = FVector(0,0,0));
	static void FrontLeft(FString name, FVector size, FVector scale, float buffer,
		FModelParams modelParams, FVector offset = FVector(0,0,0));
	static void BackLeft(FString name, FVector size, FVector scale, float buffer,
		FModelParams modelParams, FVector offset = FVector(0,0,0));
	static void Right(FString name, FVector size, FVector scale, float buffer,
		FModelParams modelParams, FVector offset = FVector(0,0,0));
	static void Left(FString name, FVector size, FVector scale, float buffer,
		FModelParams modelParams, FVector offset = FVector(0,0,0));
	static void Front(FString name, FVector size, FVector scale, float buffer,
		FModelParams modelParams, FVector offset = FVector(0,0,0));
	static void Back(FString name, FVector size, FVector scale, float buffer,
		FModelParams modelParams, FVector offset = FVector(0,0,0));
};
