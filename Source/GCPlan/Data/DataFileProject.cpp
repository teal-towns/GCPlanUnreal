#include "DataFileProject.h"

#include "JsonObjectConverter.h"

#include "../DataStructsActor.h"
#include "../Common/DataConvert.h"

DataFileProject* DataFileProject::pinstance_{nullptr};
std::mutex DataFileProject::mutex_;

DataFileProject::DataFileProject() {
}

DataFileProject::~DataFileProject() {
}

DataFileProject *DataFileProject::GetInstance() {
	std::lock_guard<std::mutex> lock(mutex_);
	if (pinstance_ == nullptr) {
		pinstance_ = new DataFileProject();
	}
	return pinstance_;
}

std::tuple<FDataProjectJson, bool> DataFileProject::LoadProject(FString fileName) {
	FDataProjectJson* data = new FDataProjectJson();
	if (fileName.Len() > 0) {
		FString filePath = "Source/Conditional/" + fileName;
		FString projectPath = FPaths::ConvertRelativePathToFull(FPaths::ProjectDir());
		filePath = projectPath + filePath;

		auto [jsonString, valid, msg] = DataConvert::ReadStringFromFile(filePath);
		if (valid) {
			FDataProjectJsonSimplified* dataSimplified = new FDataProjectJsonSimplified();
			if (!FJsonObjectConverter::JsonObjectStringToUStruct(jsonString, dataSimplified, 0, 0)) {
				UE_LOG(LogTemp, Error, TEXT("DataFileProject.LoadProject json parse error"));
			} else {
				return { ADataStructsActor::ProjectFromSimplified(*dataSimplified), true };
			}
		}
	}
	return { *data, false };
}

bool DataFileProject::SaveProject(FDataProjectJson json, FString fileName) {
	if (fileName.Len() > 0) {
		FString filePath = "Source/Conditional/" + fileName;
		FString projectPath = FPaths::ConvertRelativePathToFull(FPaths::ProjectDir());
		filePath = projectPath + filePath;

		FDataProjectJsonSimplified dataSimplified = ADataStructsActor::ProjectToSimplified(json);
		FString jsonString;
		if (!FJsonObjectConverter::UStructToJsonObjectString(dataSimplified, jsonString, 0, 0)) {
			UE_LOG(LogTemp, Error, TEXT("DataFileProject.SaveProject jsonString error"));
		} else {
			auto [valid, msg] = DataConvert::WriteStringToFile(filePath, jsonString);
			return valid;
		}
	}
	return false;
}

