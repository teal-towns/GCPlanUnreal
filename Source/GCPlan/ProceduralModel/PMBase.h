#pragma once

#include <mutex>
#include "Engine/StaticMeshActor.h"
#include "ProceduralMeshComponent.h"

#include "../ModelingStructsActor.h"
#include "../Modeling/ModelBase.h"

struct FPlaneOffsets {
	float ratioFromStart;
	// TMap<FString, float> offsetRatios = {
	// 	{ "x": 0 },
	// 	{ "y": 0 },
	// 	{ "z": 0 },
	// };
	// TMap<FString, float> offsets = {
	// 	{ "x": 0 },
	// 	{ "y": 0 },
	// 	{ "z": 0 },
	// };
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
	FPlaneOffsets(float ratioFromStart_, float offsetRatio_ = 0, TArray<FString> offsetRatioKeys_ = {},
		float offset_ = 0, TArray<FString> offsetKeys_ = {}) {
		ratioFromStart = ratioFromStart_;
		offsetXRatio = offsetRatioKeys_.Contains("x") ? offsetRatio_ : 0;
		offsetYRatio = offsetRatioKeys_.Contains("y") ? offsetRatio_ : 0;
		offsetZRatio = offsetRatioKeys_.Contains("z") ? offsetRatio_ : 0;
		offsetX = offsetKeys_.Contains("x") ? offset_ : 0;
		offsetY = offsetKeys_.Contains("y") ? offset_ : 0;
		offsetZ = offsetKeys_.Contains("z") ? offset_ : 0;
	}
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
