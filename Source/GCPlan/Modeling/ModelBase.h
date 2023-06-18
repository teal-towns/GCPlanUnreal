#pragma once

#include <mutex>
#include "Engine/StaticMeshActor.h"
#include "ProceduralMeshComponent.h"

#include "../ModelingStructsActor.h"

struct FModelParams {
	FString meshPath = "";
	FString meshKey = "";
	FString materialPath = "";
	FString materialKey = "";
	UStaticMesh* mesh = nullptr;
	UMaterialInstanceDynamic* dynamicMaterial = nullptr;
	USceneComponent* parent = nullptr;
	FString textureBase = "";
	FString textureNormal = "";
	FLinearColor textureColor = FLinearColor(1,1,1,1);
	float textureColorIntensity = 1;
	FVector rotation = FVector(0,0,0);
	FVector location = FVector(0,0,0);
};

struct FModelCreateParams {
	// FVector size;
	// TArray<FString> tags = {};
	AActor* parentActor = nullptr;
	USceneComponent* parent = nullptr;
	// UProceduralMeshComponent* proceduralMesh = nullptr;
	FVector offset = FVector(0,0,0);
	FString trianglesOrder = "";
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
	AStaticMeshActor* CreateActor(FString name, FVector location = FVector(0,0,0),
		FVector rotation = FVector(0,0,0), FVector scale = FVector(1,1,1),
		FActorSpawnParameters spawnParams = FActorSpawnParameters(), FModelParams = FModelParams());
	static void SetTransform(AActor* actor, FVector location = FVector(0,0,0),
		FVector rotation = FVector(0,0,0), FVector scale = FVector(1,1,1));

	void SetInputs(FModelingBase);
	std::tuple<FModelingBase, FModelParams> GetInputs(FString defaultName, FVector defaultSize);
	void DestroyActors();
	void CleanUp();

private:
	static ModelBase *pinstance_;
	static std::mutex mutex_;

	UWorld* World;
	FModelingBase _modelingBase;
	TMap<FString, AStaticMeshActor*> _spawnedActors = {};
};
