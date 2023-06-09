#pragma once

#include "../BuildingStructsActor.h"

class DataConvert {
public:
	DataConvert();
	~DataConvert();

	static FMapStringFloat VectorToDict(FVector vector);
	static FVector DictToVector(FMapStringFloat dict);
	static TArray<FMapStringFloat> VectorsToDicts(TArray<FVector> vectors);
	static TArray<FVector> DictsToVectors(TArray<FMapStringFloat> dicts);
	static FMapStringFloat RotatorToDict(FRotator vector);
	static FRotator DictToRotator(FMapStringFloat dict);

	static std::tuple<FString, bool, FString> ReadStringFromFile(FString FilePath);
	static std::tuple<bool, FString> WriteStringToFile(FString FilePath, FString String);
};
