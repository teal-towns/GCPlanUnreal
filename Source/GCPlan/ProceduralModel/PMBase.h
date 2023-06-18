#pragma once

#include <mutex>
#include "Engine/StaticMeshActor.h"
#include "ProceduralMeshComponent.h"

#include "../ModelingStructsActor.h"
#include "../Modeling/ModelBase.h"

struct FPlaneOffsets {
	float ratioFromStart;
	float offsetXRatio = 0;
	float offsetYRatio = 0;
	float offsetZRatio = 0;
	float offsetX = 0;
	float offsetY = 0;
	float offsetZ = 0;

	FPlaneOffsets() {};
	FPlaneOffsets(float ratioFromStart_, float offsetXRatio_ = 0, float offsetYRatio_= 0, float offsetZRatio_ = 0,
		float offsetX_ = 0, float offsetY_ = 0, float offsetZ_ = 0) {
		ratioFromStart = ratioFromStart_;
		offsetXRatio = offsetXRatio_;
		offsetYRatio = offsetYRatio_;
		offsetZRatio = offsetZRatio_;
		offsetX = offsetX_;
		offsetY = offsetY_;
		offsetZ = offsetZ_;
	};
};

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
	std::tuple<FProceduralModelBase, FModelParams> GetInputs(FString defaultName, FVector defaultSize,
		int32 defaultSidesSegmentCount = 4, float defaultTopOffsetWidth = 0.0);
	void Create();
	void CleanUp();

	static void AddMesh(AStaticMeshActor* actor, UStaticMesh* mesh, FString materialPath = "");
	static UProceduralMeshComponent* CreateMesh(FString name, UObject* parentObject,
		USceneComponent* parent = nullptr);
	static std::tuple<UProceduralMeshComponent*, AActor*> GetMesh();
	static void DestroyMesh(AActor* actor, UProceduralMeshComponent* proceduralMesh);
	static void AddMeshSection(UProceduralMeshComponent* ProceduralMesh, TArray<FVector> Vertices,
		TArray<FVector2D> UV0, TArray<int> Triangles, TArray<FVector> Normals = {},
		TArray<FProcMeshTangent> Tangents = {}, FModelParams modelParams = FModelParams());
	static UStaticMesh* ToStaticMesh(UProceduralMeshComponent* ProceduralMesh);
	static AStaticMeshActor* MeshToActor(FString name, UProceduralMeshComponent* proceduralMesh,
		FModelCreateParams createParams, FModelParams modelParams = FModelParams());

private:
	static PMBase *pinstance_;
	static std::mutex mutex_;

	UWorld* World;
	FProceduralModelBase _proceduralModelBase;
};
