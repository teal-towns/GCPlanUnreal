#pragma once

#include <mutex>

#include "../DataStructsActor.h"
#include "../SocketActor.h"

class UnrealGlobal {

private:
	static UnrealGlobal *pinstance_;
	static std::mutex mutex_;

	UWorld* World;
	TMap<FString, bool> _initeds = {};

public:
	UnrealGlobal();
	~UnrealGlobal();

	// Singletons should not be cloneable or assignable.
	UnrealGlobal(UnrealGlobal &other) = delete;
	void operator=(const UnrealGlobal &) = delete;

	FDataSettings* _settings;
	ASocketActor* SocketActor;
	TMap<FString, AActor*> _actors;

	static UnrealGlobal *GetInstance();
	void InitAll(UWorld*, TArray<FString> skipKeys = {});
	void InitCommon(UWorld*);
	void InitWeb(UWorld*);
	void InitMeshes(UWorld*);
	void SetInited(FString key);
	bool IsIniteds(TArray<FString>);
	void GetSocket(UWorld*);
	FDataSettings* Settings();
	void CleanUp(TArray<FString> skipKeys = {});
	void SetWorld(UWorld*);
	UWorld* GetWorld();
	float GetScale();
	static FVector Scale(FVector);
	void SetActorFolder(AActor* actor, FString path = "Spawned");
	void RemoveAttachedActors(AActor* actor);
	// bool RemoveActorByName(FString name, TSubclassOf<AActor> ActorClass, bool removeAttached = true);
	AActor* GetActorByName(FString name, TSubclassOf<AActor> ActorClass, bool save = false, bool matchStartsWith = false);
	static std::tuple<FDataSettings*, bool> LoadSettings(FString fileName = "settings.json");
};
