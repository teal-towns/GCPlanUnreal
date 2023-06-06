#pragma once

#include <mutex>
#include "Engine/StaticMeshActor.h"

// #include "../ModelingStructsActor.h"

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
	void AddMesh(FString name, FString meshPath = "", FString materialPath = "");
	void CreateInstance(FString uName, FVector Translation, FRotator Rotation, FVector Scale);
	void ClearInstances(FString uName);

private:
	static InstancedMesh *pinstance_;
	static std::mutex mutex_;

	static AStaticMeshActor* _instancedMeshesActor;

	// TMap<FString, AStaticMeshActor*> _instancedMeshActors = {};
	TMap<FString, AActor*> _instancedMeshActors = {};
	UWorld* World;
};
