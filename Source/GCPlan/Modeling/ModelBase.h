#pragma once

#include <mutex>
#include "Engine/StaticMeshActor.h"

#include "../ModelingStructsActor.h"

class ModelBase {
public:
	ModelBase();
	~ModelBase();

	// Singletons should not be cloneable or assignable.
	ModelBase(ModelBase &other) = delete;
	void operator=(const ModelBase &) = delete;
	static ModelBase *GetInstance();

	void SetWorld(UWorld*);
	UWorld* GetWorld();
	void Create();
	AStaticMeshActor* CreateActor(FString name, FVector location, FRotator rotation,
		FVector scale, FActorSpawnParameters spawnParams, USceneComponent* parent = nullptr,
		FString meshPath = "", FString materialPath = "");

	void SetInputs(FModelingBase);
	FModelingBase GetInputs(FString defaultName, FVector defaultSize);

private:
	static ModelBase *pinstance_;
	static std::mutex mutex_;

	UWorld* World;
	FModelingBase _modelingBase;
};
