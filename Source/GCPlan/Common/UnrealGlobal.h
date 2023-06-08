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
	TMap<FString, AActor*> _actors;

	static UnrealGlobal *GetInstance();
	void InitAll(UWorld*, TArray<FString> skipKeys = {});
	void InitCommon(UWorld*);
	void InitWeb(UWorld*);
	void InitMeshes(UWorld*);
	void SetWorld(UWorld*);
	UWorld* GetWorld();
	float GetScale();
	static FVector Scale(FVector);
	void SetActorFolder(AActor* actor, FString path = "Spawned");
	void RemoveAttachedActors(AActor* actor);
	// bool RemoveActorByName(FString name, TSubclassOf<AActor> ActorClass, bool removeAttached = true);
	AActor* GetActorByName(FString name, TSubclassOf<AActor> ActorClass, bool save = false, bool matchStartsWith = false);
};