#include "UnrealGlobal.h"

#include "Kismet/GameplayStatics.h"

#include "../Landscape/HeightMap.h"
#include "../Landscape/SplineRoad.h"
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

void UnrealGlobal::InitAll(UWorld* World1) {
	InitCommon(World1);
	InitMeshes(World1);
	InitWeb(World1);
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

	// TODO - init from JSON so meshes and materials are dynamic / per environment.
	// Must be after modelBase as uses that.
	InstancedMesh* instancedMesh = InstancedMesh::GetInstance();
	instancedMesh->SetWorld(World);
	instancedMesh->AddMesh("HexModule", "/Script/Engine.StaticMesh'/Game/Buildings/Hex/HexModule/HexModule.HexModule'",
		"/Script/Engine.Material'/Game/Nature/Wood/wood-pale-material.wood-pale-material'");
	instancedMesh->AddMesh("RoadRoundabout", "/Script/Engine.StaticMesh'/Game/Landscape/Roundabout1.Roundabout1'",
		"/Script/Engine.Material'/Game/Landscape/Asphalt_M.Asphalt_M'");
}

void UnrealGlobal::SetWorld(UWorld* World1) {
	World = World1;
}

UWorld* UnrealGlobal::GetWorld() {
	return World;
}

float UnrealGlobal::Scale() {
	return 100;
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

AActor* UnrealGlobal::GetActorByName(FString name, TSubclassOf<AActor> ActorClass) {
	AActor* actor = nullptr;
	TArray<AActor*> OutActors;
	UGameplayStatics::GetAllActorsOfClass(World, ActorClass, OutActors);
	for (AActor* a : OutActors) {
		if (a->GetName() == name) {
			return a;
		}
	}
	return actor;
}
