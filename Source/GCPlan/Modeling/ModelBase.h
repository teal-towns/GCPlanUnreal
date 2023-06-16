#pragma once

#include <mutex>
#include "Engine/StaticMeshActor.h"

#include "../ModelingStructsActor.h"

struct FModelParams {
	FString meshPath = "";
	FString materialPath = "";
	UStaticMesh* mesh = nullptr;
	UMaterialInstanceDynamic* dynamicMaterial = nullptr;
	USceneComponent* parent = nullptr;
	FString textureBase = "";
	FString textureNormal = "";
	FLinearColor textureColor = FLinearColor(255,255,255);
	float textureColorIntensity = 1;
};

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
	void CreateFloor();
	// AStaticMeshActor* CreateActor(FString name, FVector location, FRotator rotation,
	// 	FVector scale, FActorSpawnParameters spawnParams, USceneComponent* parent = nullptr,
	// 	FString meshPath = "", FString materialPath = "", UStaticMesh* mesh = nullptr,
	// 	FModelParams = FModelParams());
	AStaticMeshActor* CreateActor(FString name, FVector location, FRotator rotation,
		FVector scale, FActorSpawnParameters spawnParams, FModelParams = FModelParams());

	void SetInputs(FModelingBase);
	FModelingBase GetInputs(FString defaultName, FVector defaultSize);
	void DestroyActors();
	void CleanUp();

private:
	static ModelBase *pinstance_;
	static std::mutex mutex_;

	UWorld* World;
	FModelingBase _modelingBase;
	TMap<FString, AStaticMeshActor*> _spawnedActors = {};
};
