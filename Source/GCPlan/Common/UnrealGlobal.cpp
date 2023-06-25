#include "UnrealGlobal.h"

#include "JsonObjectConverter.h"
#include "Kismet/GameplayStatics.h"

#include "../DataStructsActor.h"
#include "../Common/DataConvert.h"
#include "../Landscape/HeightMap.h"
#include "../Landscape/SplineRoad.h"
#include "../Landscape/VerticesEdit.h"
#include "../Mesh/InstancedMesh.h"
#include "../Mesh/LoadContent.h"
#include "../Modeling/ModelBase.h"
#include "../ProceduralModel/PMBase.h"

UnrealGlobal* UnrealGlobal::pinstance_{nullptr};
std::mutex UnrealGlobal::mutex_;

UnrealGlobal::UnrealGlobal() {
}

UnrealGlobal::~UnrealGlobal() {
}

UnrealGlobal *UnrealGlobal::GetInstance() {
	std::lock_guard<std::mutex> lock(mutex_);
	if (pinstance_ == nullptr) {
		pinstance_ = new UnrealGlobal();
	}
	return pinstance_;
}

void UnrealGlobal::InitAll(UWorld* World1, TArray<FString> skipKeys) {
	SetWorld(World1);
	// CleanUp({ "socket"} );

	ModelBase* modelBase = ModelBase::GetInstance();
	modelBase->SetWorld(World1);
	PMBase* pmBase = PMBase::GetInstance();
	pmBase->SetWorld(World1);

	if (_settings == nullptr) {
		// Init some first where order matters (latter ones depend on these).
		auto [dataSettings, valid] = LoadSettings();
		if (valid) {
			_settings = dataSettings;

		    if (!skipKeys.Contains("meshes")) {
				InitMeshes(World1);
			}

			InitCommon(World1);

			if (!skipKeys.Contains("web")) {
				InitWeb(World1);
			}
		}
	}
}

void UnrealGlobal::InitCommon(UWorld* World1) {
	SetWorld(World1);

    HeightMap* heightMap = HeightMap::GetInstance();
    heightMap->SetWorld(World1);

    SplineRoad* splineRoad = SplineRoad::GetInstance();
    splineRoad->SetWorld(World1);
}

void UnrealGlobal::InitWeb(UWorld* World1) {
	SetWorld(World1);
	GetSocket(World1);
	SocketActor->InitSocket();
}

void UnrealGlobal::GetSocket(UWorld* World1) {
	TArray<AActor*> OutActors;
	UGameplayStatics::GetAllActorsOfClass(World1, ASocketActor::StaticClass(), OutActors);
	for (AActor* a : OutActors) {
		SocketActor = Cast<ASocketActor>(a);
		break;
	}
}

FDataSettings* UnrealGlobal::Settings() {
	if (!_settings) {
		UE_LOG(LogTemp, Warning, TEXT("UnrealGlobal.Settings no settings"));
		return nullptr;
	}
	return _settings;
}

void UnrealGlobal::InitMeshes(UWorld* World1) {
	SetWorld(World1);

	// Must be after modelBase as uses that.
	InstancedMesh* instancedMesh = InstancedMesh::GetInstance();
	instancedMesh->SetWorld(World);
	instancedMesh->InitMeshes();

	// Load from JSON too.
	LoadContent* loadContent = LoadContent::GetInstance();
	loadContent->LoadMeshes();
}

void UnrealGlobal::SetInited(FString key) {
	_initeds.Add(key, true);
}

bool UnrealGlobal::IsIniteds(TArray<FString> Keys) {
	for (int ii = 0; ii < Keys.Num(); ii++) {
		if (!_initeds.Contains(Keys[ii]) || !_initeds[Keys[ii]]) {
			return false;
		}
	}
	return true;
}

void UnrealGlobal::CleanUp(TArray<FString> skipKeys) {
	InstancedMesh* instancedMesh = InstancedMesh::GetInstance();
	instancedMesh->CleanUp();

	SplineRoad* splineRoad = SplineRoad::GetInstance();
	splineRoad->CleanUp();

	VerticesEdit* verticesEdit = VerticesEdit::GetInstance();
    verticesEdit->CleanUp();

    if (!skipKeys.Contains("socket") && IsValid(SocketActor)) {
		SocketActor->Destroy();
	}
	_initeds.Empty();
	_actors.Empty();
	// World = nullptr;
	_settings = nullptr;
	SocketActor = nullptr;
}

void UnrealGlobal::SetWorld(UWorld* World1) {
	World = World1;
}

UWorld* UnrealGlobal::GetWorld() {
	return World;
}

float UnrealGlobal::GetScale() {
	return 100;
}

FVector UnrealGlobal::Scale(FVector location) {
	return location * 100;
}

void UnrealGlobal::SetActorFolder(AActor* actor, FString path) {
#if WITH_EDITOR
	if (actor) {
    	actor->SetFolderPath(FName(*path));
    } else {
    	UE_LOG(LogTemp, Warning, TEXT("UnrealGlobal.SetActorFolder actor invalid, skipping %s"), *path);
    }
#endif
}

void UnrealGlobal::RemoveAttachedActors(AActor* actor) {
	TArray<AActor*> OutActors;
	if (actor) {
		actor->GetAttachedActors(OutActors);
		for (AActor* a : OutActors) {
			if (IsValid(a)) {
				a->Destroy();
			}
		}
	}
}

AActor* UnrealGlobal::GetActorByName(FString name, TSubclassOf<AActor> ActorClass, bool save, bool matchStartsWith) {
	// UE_LOG(LogTemp, Display, TEXT("GetActorByName %s"), *name);
	if (_actors.Contains(name)) {
		return _actors[name];
	}

	AActor* actor = nullptr;
	TArray<AActor*> OutActors;
	FString nameTemp;
	UGameplayStatics::GetAllActorsOfClass(World, ActorClass, OutActors);
	for (AActor* a : OutActors) {
		nameTemp = a->GetName();
		if (nameTemp == name || (matchStartsWith && nameTemp.StartsWith(name, ESearchCase::CaseSensitive))) {
			if (save) {
				_actors.Add(name, a);
			}
			return a;
		}
	}
	return actor;
}

std::tuple<FDataSettings*, bool> UnrealGlobal::LoadSettings(FString fileName) {
	FDataSettings* data = new FDataSettings();
	if (fileName.Len() > 0) {
		FString filePath = "Source/Conditional/" + fileName;
		FString projectPath = FPaths::ConvertRelativePathToFull(FPaths::ProjectDir());
		filePath = projectPath + filePath;

		DataConvert* dataConvert = DataConvert::GetInstance();
		auto [jsonString, valid, msg] = dataConvert->ReadStringFromFile(filePath);
		if (valid) {
			if (!FJsonObjectConverter::JsonObjectStringToUStruct(jsonString, data, 0, 0)) {
				UE_LOG(LogTemp, Error, TEXT("UnrealGlobal.LoadSettings json parse error"));
			} else {
				dataConvert->SetProjectPath(data->projectPath);
				return { data, true };
			}
		}
	}
	return { data, false };
}
