#include "DataConvert.h"

#include "../BuildingStructsActor.h"

DataConvert::DataConvert() {
}

DataConvert::~DataConvert() {
}

FMapStringFloat DataConvert::VectorToDict(FVector vector) {
	return FMapStringFloat(TMap<FString, float> {
		{ "x", vector.X },
		{ "y", vector.Y },
		{ "z", vector.Z },
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
		{ "x", vector.Roll },
		{ "y", vector.Pitch },
		{ "z", vector.Yaw },
	});
}

FRotator DataConvert::DictToRotator(FMapStringFloat dict) {
	return FRotator(dict.v["x"], dict.v["y"], dict.v["z"]);
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
