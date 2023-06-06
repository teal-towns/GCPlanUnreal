#pragma once

#include <mutex>

#include "../SettingsActor.h"
#include "../SocketActor.h"

class UnrealGlobal {

private:
	static UnrealGlobal *pinstance_;
	static std::mutex mutex_;

	UWorld* World;

public:
	UnrealGlobal();
	~UnrealGlobal();

	// Singletons should not be cloneable or assignable.
	UnrealGlobal(UnrealGlobal &other) = delete;
	void operator=(const UnrealGlobal &) = delete;

	ASettingsActor* SettingsActor;
	ASocketActor* SocketActor;

	static UnrealGlobal *GetInstance();
	void InitAll(UWorld*);
	void InitCommon(UWorld*);
	void InitWeb(UWorld*);
	void InitMeshes(UWorld*);
	void SetWorld(UWorld*);
	UWorld* GetWorld();
	float Scale();
	void SetActorFolder(AActor* actor, FString path = "Spawned");
	void RemoveAttachedActors(AActor* actor);
	// bool RemoveActorByName(FString name, TSubclassOf<AActor> ActorClass, bool removeAttached = true);
	AActor* GetActorByName(FString name, TSubclassOf<AActor> ActorClass);
};
