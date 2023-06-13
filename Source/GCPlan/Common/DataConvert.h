#pragma once

#include <mutex>

#include "../BuildingStructsActor.h"

class DataConvert {
private:
	static DataConvert *pinstance_;
	static std::mutex mutex_;

	FString _projectPath = "";

public:
	DataConvert();
	~DataConvert();

	// Singletons should not be cloneable or assignable.
	DataConvert(DataConvert &other) = delete;
	void operator=(const DataConvert &) = delete;

	static DataConvert *GetInstance();

	static FMapStringFloat VectorToDict(FVector vector);
	static FVector DictToVector(FMapStringFloat dict);
	static TArray<FMapStringFloat> VectorsToDicts(TArray<FVector> vectors);
	static TArray<FVector> DictsToVectors(TArray<FMapStringFloat> dicts);
	static FMapStringFloat RotatorToDict(FRotator vector);
	static FRotator DictToRotator(FMapStringFloat dict);
	static FRotator VectorToRotator(FVector vector);

	void SetProjectPath(FString);
	FString FileNameToPath(FString fileName, FString key);
	std::tuple<FString, bool, FString> ReadFile(FString fileName, FString filePathKey = "conditional");
	std::tuple<FString, bool, FString> ReadStringFromFile(FString FilePath);
	std::tuple<bool, FString> WriteStringToFile(FString FilePath, FString String);
	std::tuple<bool, FString> WriteFile(FString fileName, FString String, FString filePathKey = "conditional");
};
