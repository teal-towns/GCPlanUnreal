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
	// float textureColorIntensity = 1;
	FVector rotation = FVector(0,0,0);
	FVector location = FVector(0,0,0);

	// FModelParams() {};
	// FModelParams(FModelParams input) {
	// 	meshPath = input.meshPath;
	// 	meshKey = input.meshKey;
	// 	materialPath = input.materialPath;
	// 	materialKey = input.materialKey;
	// 	mesh = input.mesh;
	// 	dynamicMaterial = input.dynamicMaterial;
	// 	parent = input.parent;
	// 	textureBase = input.textureBase;
	// 	textureNormal = input.textureNormal;
	// 	textureColor = input.textureColor;
	// 	rotation = input.rotation;
	// 	location = input.location;
	// };
};

struct FModelCreateParams {
	// FVector size;
	AActor* parentActor = nullptr;
	USceneComponent* parent = nullptr;
	// UProceduralMeshComponent* proceduralMesh = nullptr;
	FVector offset = FVector(0,0,0);
	FVector rotation = FVector(0,0,0);
	FVector rotateAround = FVector(0,0,0);
	FString triangleOrder = "";
	int triangleSide = 1;
	TMap<FString, FString> offsetsCombine = {};
	FVector2D UVScale = FVector2D(1,1);

	// FModelCreateParams() {};
	// FModelCreateParams(FModelCreateParams input, AActor parentActor_ = nullptr,
	// 	USceneComponent* parent_ = nullptr) {
	// 	parentActor = parentActor_ ? parentActor_ : input.parentActor;
	// 	parent = parent_ ? parent_ : input.parent;
	// 	// offset = input.offset;
	// 	// rotation = input.rotation;
	// 	// triangleOrder = input.triangleOrder;
	// 	// triangleSide = input.triangleSide;
	// 	// offsetsCombine = input.offsetsCombine;
	// 	// UVScale = input.UVScale;
	// };
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
	static FString CheckGetName(FString name = "", FString defaultName = "");
	AStaticMeshActor* CreateActorEmpty(FString name, FModelParams modelParams);
	AStaticMeshActor* CreateActor(FString name, FVector location = FVector(0,0,0),
		FVector rotation = FVector(0,0,0), FVector scale = FVector(1,1,1),
		FActorSpawnParameters spawnParams = FActorSpawnParameters(), FModelParams = FModelParams());
	static void SetMeshMaterialFromParams(UStaticMeshComponent* meshComponent, FModelParams modelParams);
	static std::tuple<FVector, FVector, FVector> PairsToTransform(TMap<FString, FString> pairs,
		FVector scale = FVector(1,1,1));
	static std::tuple<FString, FModelParams> ModelParamsFromPairs(TMap<FString, FString> pairs);
	static FString InstancedMeshFromPairs(TMap<FString, FString> pairs);
	static FString AddRotationString(FVector rotationParent, FVector rotation = FVector(0,0,0),
		FString meshKey = "");
	static TArray<FVector> Vertices(TArray<FVector> vertices, FModelCreateParams createParams,
		FVector rotation);
	static void SetTransformFromParams(AActor* actor, FModelCreateParams createParams);
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
