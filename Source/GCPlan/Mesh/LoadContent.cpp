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
	DataConvert* dataConvert = DataConvert::GetInstance();
	auto [jsonString, valid, msg] = dataConvert->ReadFile(fileName, "conditional");
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
	auto [data, valid] = LoadFile(unrealGlobal->Settings()->jsonFiles["contentMeshes"]);
	if (valid) {
		_materialPaths = data.materials;
		_texturePaths = data.textures;
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

FString LoadContent::Material(FString key) {
	if (_materialPaths.Contains(key)) {
		return _materialPaths[key];
	}
	return "";
}

FString LoadContent::Texture(FString key) {
	if (_texturePaths.Contains(key)) {
		return _texturePaths[key];
	}
	return "";
}

FString LoadContent::Mesh(FString key) {
	if (_meshModels.Contains(key)) {
		return _meshModels[key].path;
	}
	return "";
}
