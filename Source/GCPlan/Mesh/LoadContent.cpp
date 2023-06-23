#include "LoadContent.h"

#include "JsonObjectConverter.h"

#include "../DataStructsActor.h"
#include "../Common/DataConvert.h"
#include "../Common/Lodash.h"
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
		for (auto& Elem : _meshModels) {
			name = Elem.Key;
			meshPath = Elem.Value.path;
			materialPath = Elem.Value.materialPath;
			mesh = Cast<UStaticMesh>(StaticLoadObject(UStaticMesh::StaticClass(), NULL, *meshPath));
			_meshModels[Elem.Key].tags = Lodash::CheckAddSuffix(_meshModels[Elem.Key].tags, ",");
			// UE_LOG(LogTemp, Display, TEXT("meshPath %s name %s tags %s"), *meshPath, *name, *_meshModels[Elem.Key].tags);
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

TArray<FString> LoadContent::GetMeshNamesByTags(TArray<FString> tags) {
	TArray<FString> names = {};
	for (auto& Elem : _meshModels) {
		if (Elem.Value.tags.Len() > 0) {
			for (int ii = 0; ii < tags.Num(); ii++) {
				if (Elem.Value.tags.Contains(tags[ii] + ",")) {
					names.Add(Elem.Key);
					break;
				}
			}
		}
	}
	return names;
}

std::tuple<bool, TMap<FString, TArray<FString>>> LoadContent::FillMeshesByTags(TMap<FString, TArray<FString>> meshesByTags,
	TArray<FString> required) {
	bool valid = true;
	TArray<FString> tags;
	FString tag;
	for (auto& Elem : meshesByTags) {
		if (Elem.Value.Num() < 1) {
			tag = Elem.Key;
			tags = { tag };
			meshesByTags[Elem.Key] = GetMeshNamesByTags(tags);
			if ((required.Contains(tag) || required.Contains("[all]")) && meshesByTags[Elem.Key].Num() < 1) {
				UE_LOG(LogTemp, Display, TEXT("LoadContent.FillMeshesByTags missing meshes for tag %s"), *tag);
				valid = false;
				break;
			}
		}
	}
	return { valid, meshesByTags };
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

FString LoadContent::MeshScale(FVector scale, FString meshKey, TArray<FString> axes) {
	if (meshKey.Len() > 0 && _meshModels.Contains(meshKey) && _meshModels[meshKey].size.Len() > 0) {
		FVector meshSize = DataConvert::StringToVector(_meshModels[meshKey].size);
		if (axes.Contains("x")) {
			scale.X = scale.X / meshSize.X;
		}
		if (axes.Contains("y")) {
			scale.Y = scale.Y / meshSize.Y;
		}
		if (axes.Contains("z")) {
			scale.Z = scale.Z / meshSize.Z;
		}
	}
	return DataConvert::VectorToString(scale);
}

FVector LoadContent::MeshRotation(FString meshKey) {
	if (_meshModels.Contains(meshKey)) {
		return DataConvert::StringToVector(_meshModels[meshKey].rotation);
	}
	return FVector(0,0,0);
}
