#include "LoadContent.h"

#include "JsonObjectConverter.h"

#include "../DataStructsActor.h"
#include "../Common/DataConvert.h"
#include "../Common/UnrealGlobal.h"
#include "../Mesh/InstancedMesh.h"

LoadContent* LoadContent::pinstance_{nullptr};
std::mutex LoadContent::mutex_;

LoadContent::LoadContent() {
}

LoadContent::~LoadContent() {
}

LoadContent *LoadContent::GetInstance() {
	std::lock_guard<std::mutex> lock(mutex_);
	if (pinstance_ == nullptr) {
		pinstance_ = new LoadContent();
	}
	return pinstance_;
}

void LoadContent::CleanUp() {
	_meshModels.Empty();
}

std::tuple<FDataContentMesh, bool> LoadContent::LoadFile(FString fileName) {
	FDataContentMesh* data = new FDataContentMesh();
	auto [jsonString, valid, msg] = DataConvert::ReadFile(fileName, "conditional");
	if (valid) {
		if (!FJsonObjectConverter::JsonObjectStringToUStruct(jsonString, data, 0, 0)) {
			UE_LOG(LogTemp, Error, TEXT("LoadContent.LoadFile json parse error"));
		} else {
			return { *data, true };
		}
	}
	return { *data, false };
}

void LoadContent::LoadMeshes(FString defaultMeshPath) {
	UnrealGlobal* unrealGlobal = UnrealGlobal::GetInstance();
	auto [data, valid] = LoadFile(unrealGlobal->Settings()->contentMeshesJsonFile);
	if (valid) {
		InstancedMesh* instancedMesh = InstancedMesh::GetInstance();
		UStaticMesh* mesh;
		FString meshPath, name, materialPath;
		_meshModels = data.models;
		for (auto& Elem : data.models) {
			name = Elem.Key;
			meshPath = Elem.Value.path;
			materialPath = Elem.Value.materialPath;
			mesh = Cast<UStaticMesh>(StaticLoadObject(UStaticMesh::StaticClass(), NULL, *meshPath));
			UE_LOG(LogTemp, Display, TEXT("meshPath %s"), *meshPath);
			if (!mesh) {
				UE_LOG(LogTemp, Display, TEXT("LoadContent.LoadMeshes not found, using defaultMeshPath %s"), *name);
				meshPath = defaultMeshPath;
			}
			instancedMesh->AddMesh(name, meshPath, materialPath);
		}
	}
}

TArray<FString> LoadContent::GetMeshNamesByTypes(TArray<FString> types) {
	TArray<FString> names = {};
	for (auto& Elem : _meshModels) {
		if (types.Contains(Elem.Value.type)) {
			names.Add(Elem.Key);
		}
	}
	return names;
}
