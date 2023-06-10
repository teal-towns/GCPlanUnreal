#include "DataConvert.h"

#include "JsonObjectConverter.h"
#include "HAL/PlatformFileManager.h"
#include "Misc/FileHelper.h"

#include "../BuildingStructsActor.h"

DataConvert::DataConvert() {
}

DataConvert::~DataConvert() {
}

FMapStringFloat DataConvert::VectorToDict(FVector vector) {
	return FMapStringFloat(TMap<FString, float> {
		{ "x", (float)vector.X },
		{ "y", (float)vector.Y },
		{ "z", (float)vector.Z },
	});
}

FVector DataConvert::DictToVector(FMapStringFloat dict) {
	return FVector(dict.v["x"], dict.v["y"], dict.v["z"]);
}

TArray<FMapStringFloat> DataConvert::VectorsToDicts(TArray<FVector> vectors) {
	TArray<FMapStringFloat> dicts = {};
	for (int ii = 0; ii < vectors.Num(); ii++) {
		dicts.Add(VectorToDict(vectors[ii]));
	}
	return dicts;
}

TArray<FVector> DataConvert::DictsToVectors(TArray<FMapStringFloat> dicts) {
	TArray<FVector> vectors = {};
	for (int ii = 0; ii < dicts.Num(); ii++) {
		vectors.Add(DictToVector(dicts[ii]));
	}
	return vectors;
}

FMapStringFloat DataConvert::RotatorToDict(FRotator vector) {
	return FMapStringFloat(TMap<FString, float> {
		{ "x", (float)vector.Roll },
		{ "y", (float)vector.Pitch },
		{ "z", (float)vector.Yaw },
	});
}

FRotator DataConvert::DictToRotator(FMapStringFloat dict) {
	return FRotator(dict.v["y"], dict.v["z"], dict.v["x"]);
}

FRotator DataConvert::VectorToRotator(FVector vector) {
	return FRotator(vector.Y, vector.Z, vector.X);
}

// TArray<float> DataConvert::VectorToArray(FVector vector) {
// 	return TArray<float> { vector.X, vector.Y, vector.Z };
// }

// FVector DataConvert::ArrayToVector(TArray<float> array1) {
// 	return FVector(array[0], array[1], array[2]);
// }

// TArray<TArray<float>> DataConvert::VectorsToArrays(TArray<FVector> vectors) {
// 	TArray<TArray<float>> arrays1 = {};
// 	for (int ii = 0; ii < vectors.Num(); ii++) {
// 		arrays1.Add(VectorToArray(vectors[ii]));
// 	}
// 	return arrays1;
// }

// TArray<FVector> DataConvert::ArraysToVectors(TArray<TArray<float>> arrays1) {
// 	TArray<FVector> vectors = {};
// 	for (int ii = 0; ii < arrays1.Num(); ii++) {
// 		vectors.Add(ArrayToVector(arrays1[ii]));
// 	}
// 	return vectors;
// }

// TArray<float> DataConvert::RotatorToArray(FRotator vector) {
// 	return TArray<float> { vector.Roll, vector.Pitch, vector.Yaw };
// }

// FRotator DataConvert::ArrayToRotator(TArray<float> array1) {
// 	return FRotator(array1[0], array1[1], array1[2]);
// }

FString DataConvert::FileNameToPath(FString fileName, FString key) {
	FString filePath = "";
	if (fileName.Len() > 0) {
		if (key == "conditional") {
			filePath = "Source/Conditional/" + fileName;
		}
		FString projectPath = FPaths::ConvertRelativePathToFull(FPaths::ProjectDir());
		filePath = projectPath + filePath;
	}
	return filePath;
}

std::tuple<FString, bool, FString> DataConvert::ReadFile(FString fileName, FString filePathKey) {
	FString filePath = FileNameToPath(fileName, filePathKey);
	if (filePath.Len() < 1) {
		return { "", false, "" };
	}
	return ReadStringFromFile(filePath);
}

std::tuple<FString, bool, FString> DataConvert::ReadStringFromFile(FString FilePath) {
	bool valid = true;
	FString msg = "";
	FString RetString = "";
	if (!FPlatformFileManager::Get().GetPlatformFile().FileExists(*FilePath)) {
		valid = false;
		UE_LOG(LogTemp, Warning, TEXT("DataFileProject.ReadStringFromFile file does not exist %s"), *FilePath);
		msg = FString::Printf(TEXT("File does not exist %s"), *FilePath);
	}
	if (!FFileHelper::LoadFileToString(RetString, *FilePath)) {
		valid = false;
		UE_LOG(LogTemp, Warning, TEXT("DataFileProject.ReadStringFromFile unable to read file %s"), *FilePath);
		msg = FString::Printf(TEXT("Unable to read file %s"), *FilePath);
	}
	return {RetString, valid, msg};
}

std::tuple<bool, FString> DataConvert::WriteStringToFile(FString FilePath, FString String) {
	bool valid = true;
	FString msg = "";
	if (!FFileHelper::SaveStringToFile(String, *FilePath, FFileHelper::EEncodingOptions::AutoDetect,
			&IFileManager::Get(), EFileWrite::FILEWRITE_None)) {
		valid = false;
		UE_LOG(LogTemp, Warning, TEXT("DataFileProject.WriteStringToFile unable to write file %s"), *FilePath);
		msg = FString::Printf(TEXT("Unable to write file %s"), *FilePath);
	}
	return {valid, msg};
}
