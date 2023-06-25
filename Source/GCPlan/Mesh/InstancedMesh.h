#pragma once

#include <mutex>
#include "Engine/StaticMeshActor.h"

#include "../Modeling/ModelBase.h"

class InstancedMesh {
public:
	InstancedMesh();
	~InstancedMesh();

	// Singletons should not be cloneable or assignable.
	InstancedMesh(InstancedMesh &other) = delete;
	void operator=(const InstancedMesh &) = delete;
	static InstancedMesh *GetInstance();

	void SetWorld(UWorld*);
	// UWorld* GetWorld();
	void CleanUp();
	void Init();
	void InitMeshes();
	// bool MeshExists(FString name);
	void AddMesh(FString name, FString meshPath = "", FString materialPath = "",
		FModelParams modelParams = FModelParams());
	int CreateInstance(FString uName, FVector Translation, FRotator Rotation = FRotator(0,0,0), FVector Scale = FVector(1,1,1),
		bool unrealScaleTranslation = true);
	int UpdateInstance(FString uName, int, FVector Translation, FRotator Rotation = FRotator(0,0,0), FVector Scale = FVector(1,1,1),
		bool unrealScaleTranslation = true);
	int SaveInstance(FString uName, int, FVector Translation, FRotator Rotation = FRotator(0,0,0), FVector Scale = FVector(1,1,1),
		bool unrealScaleTranslation = true);
	void ClearInstances(FString);
	void ClearInstancesBulk(TArray<FString>);
	bool RemoveInstances(FString meshKey, TArray<int> instanceIndices);

private:
	static InstancedMesh *pinstance_;
	static std::mutex mutex_;

	AStaticMeshActor* _instancedMeshesActor = nullptr;

	// TMap<FString, AStaticMeshActor*> _instancedMeshActors = {};
	TMap<FString, AActor*> _instancedMeshActors = {};
	UWorld* World;
};
