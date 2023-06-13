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
	DataConvert* dataConvert = DataConvert::GetInstance();
	auto [jsonString, valid, msg] = dataConvert->ReadFile(fileName);
	if (valid) {
		FDataProjectJsonSimplified* dataSimplified = new FDataProjectJsonSimplified();
		if (!FJsonObjectConverter::JsonObjectStringToUStruct(jsonString, dataSimplified, 0, 0)) {
			UE_LOG(LogTemp, Error, TEXT("DataFileProject.LoadProject json parse error"));
		} else {
			return { ADataStructsActor::ProjectFromSimplified(*dataSimplified), true };
		}
	}
	return { *data, false };
}

bool DataFileProject::SaveProject(FDataProjectJson json, FString fileName) {
	if (fileName.Len() > 0) {
		FDataProjectJsonSimplified dataSimplified = ADataStructsActor::ProjectToSimplified(json);
		FString jsonString;
		if (!FJsonObjectConverter::UStructToJsonObjectString(dataSimplified, jsonString, 0, 0)) {
			UE_LOG(LogTemp, Error, TEXT("DataFileProject.SaveProject jsonString error"));
		} else {
			DataConvert* dataConvert = DataConvert::GetInstance();
			auto [valid, msg] = dataConvert->WriteFile(fileName, jsonString);
			return valid;
		}
	}
	return false;
}
