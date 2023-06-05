#pragma once

#include <mutex>
#include "Engine/StaticMeshActor.h"
#include "ProceduralMeshComponent.h"

#include "../ModelingStructsActor.h"

class PMBase {
public:
	PMBase();
	~PMBase();

	// Singletons should not be cloneable or assignable.
	PMBase(PMBase &other) = delete;
	void operator=(const PMBase &) = delete;
	static PMBase *GetInstance();

	void SetWorld(UWorld*);
	UWorld* GetWorld();
	void SetInputs(FProceduralModelBase);
	FProceduralModelBase GetInputs(FString defaultName, FVector defaultSize, FVector defaultVertices);
	void Create();

	AStaticMeshActor* CreateActor(FString name, FVector location, FRotator rotation,
		FVector scale, FActorSpawnParameters spawnParams, USceneComponent* parent = nullptr);
	static UProceduralMeshComponent* CreateMesh(UObject* parentObject, USceneComponent* parent,
		FString name);
	static void AddMeshSection(UProceduralMeshComponent* ProceduralMesh, TArray<FVector> Vertices,
		TArray<FVector2D> UV0, TArray<int> Triangles);

private:
	static PMBase *pinstance_;
	static std::mutex mutex_;

	UWorld* World;
	FProceduralModelBase _proceduralModelBase;
};
