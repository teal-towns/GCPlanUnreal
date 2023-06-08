#include "UnrealGlobal.h"

#include "Kismet/GameplayStatics.h"

#include "../Landscape/HeightMap.h"
#include "../Landscape/SplineRoad.h"
#include "../Landscape/VerticesEdit.h"
#include "../Mesh/InstancedMesh.h"
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
	InitCommon(World1);
	InitMeshes(World1);
	if (!skipKeys.Contains("web")) {
		InitWeb(World1);
	}
}

void UnrealGlobal::InitCommon(UWorld* World1) {
	SetWorld(World1);

	ModelBase* modelBase = ModelBase::GetInstance();
    modelBase->SetWorld(World1);

    PMBase* pmBase = PMBase::GetInstance();
    pmBase->SetWorld(World1);

    HeightMap* heightMap = HeightMap::GetInstance();
    heightMap->SetWorld(World1);

    SplineRoad* splineRoad = SplineRoad::GetInstance();
    splineRoad->SetWorld(World1);

    // VerticesEdit* verticesEdit = VerticesEdit::GetInstance();
}

void UnrealGlobal::InitWeb(UWorld* World1) {
	SetWorld(World1);

	TArray<AActor*> OutActors;
	UGameplayStatics::GetAllActorsOfClass(World, ASettingsActor::StaticClass(), OutActors);
	for (AActor* a : OutActors) {
		SettingsActor = Cast<ASettingsActor>(a);
		break;
	}
	UGameplayStatics::GetAllActorsOfClass(World, ASocketActor::StaticClass(), OutActors);
	for (AActor* a : OutActors) {
		SocketActor = Cast<ASocketActor>(a);
		break;
	}

	SocketActor->InitSocket();
}

void UnrealGlobal::InitMeshes(UWorld* World1) {
	SetWorld(World1);

	// Must be after modelBase as uses that.
	InstancedMesh* instancedMesh = InstancedMesh::GetInstance();
	instancedMesh->SetWorld(World);
	instancedMesh->InitMeshes();
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
    actor->SetFolderPath(FName(*path));
#endif
}

void UnrealGlobal::RemoveAttachedActors(AActor* actor) {
	TArray<AActor*> OutActors;
	actor->GetAttachedActors(OutActors);
	for (AActor* a : OutActors) {
		a->Destroy();
	}
}

// bool UnrealGlobal::RemoveActorByName(FString name, TSubclassOf<AActor> ActorClass, bool removeAttached) {
// 	bool found = false;
// 	TArray<AActor*> OutActors;
// 	UGameplayStatics::GetAllActorsOfClass(World, ActorClass, OutActors);
// 	for (AActor* a : OutActors) {
// 		if (a->GetName() == name) {
// 			if (removeAttached) {
// 				RemoveAttachedActors(a);
// 			}
// 			a->Destroy();
// 			found = true;
// 			break;
// 		}
// 	}
// 	return found;
// }

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