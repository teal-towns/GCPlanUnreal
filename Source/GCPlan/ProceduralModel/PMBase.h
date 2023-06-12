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
	FProceduralModelBase GetInputs(FString defaultName, FVector defaultSize, FVector defaultVertices, int32 defaultSidesSegmentCount = 4, float defaultTopOffsetWidth = 0.0);
	void Create();

	AStaticMeshActor* CreateActor(FString name, FVector location,
		FRotator rotation, FActorSpawnParameters spawnParams,
		USceneComponent* parent = nullptr);
	static void AddMesh(AStaticMeshActor* actor, UStaticMesh* mesh, FString materialPath = "");
	static UProceduralMeshComponent* CreateMesh(UObject* parentObject, USceneComponent* parent,
		FString name);
	static void AddMeshSection(UProceduralMeshComponent* ProceduralMesh, TArray<FVector> Vertices,
							   TArray<FVector2D> UV0, TArray<int> Triangles, TArray<FVector> Normals = TArray<FVector>(), TArray<FProcMeshTangent> Tangents = TArray<FProcMeshTangent>());
	static UStaticMesh* ToStaticMesh(UProceduralMeshComponent* ProceduralMesh);

private:
	static PMBase *pinstance_;
	static std::mutex mutex_;

	UWorld* World;
	FProceduralModelBase _proceduralModelBase;
};
