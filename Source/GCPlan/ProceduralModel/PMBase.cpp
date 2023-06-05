#include "PMBase.h"

#include "ProceduralMeshComponent.h"

#include "../Common/Lodash.h"
#include "../ModelingStructsActor.h"

#include "PMCube.h"
#include "PMCylinder.h"

PMBase* PMBase::pinstance_{nullptr};
std::mutex PMBase::mutex_;

PMBase::PMBase() {
}

PMBase::~PMBase() {
}

PMBase *PMBase::GetInstance() {
	std::lock_guard<std::mutex> lock(mutex_);
	if (pinstance_ == nullptr) {
		pinstance_ = new PMBase();
	}
	return pinstance_;
}

void PMBase::SetWorld(UWorld* World1) {
	World = World1;
}

UWorld* PMBase::GetWorld() {
	return World;
}

void PMBase::SetInputs(FProceduralModelBase proceduralModelBase) {
	_proceduralModelBase = proceduralModelBase;
}

FProceduralModelBase PMBase::GetInputs(FString defaultName, FVector defaultSize, FVector defaultVertices) {
	FProceduralModelBase proceduralModelBase = _proceduralModelBase;
	if (proceduralModelBase.name.Len() < 1) {
		proceduralModelBase.name = defaultName;
	}
	proceduralModelBase.name = Lodash::GetInstanceId(proceduralModelBase.name + "_");

	float minSize = 0.01;
	if (proceduralModelBase.size.X < minSize || proceduralModelBase.size.X > -1 * minSize) {
		proceduralModelBase.size.X = defaultSize.X;
	}
	if (proceduralModelBase.size.Y < minSize || proceduralModelBase.size.Y > -1 * minSize) {
		proceduralModelBase.size.Y = defaultSize.Y;
	}
	if (proceduralModelBase.size.Z < minSize || proceduralModelBase.size.Z > -1 * minSize) {
		proceduralModelBase.size.Z = defaultSize.Z;
	}

	if (proceduralModelBase.vertices.X < minSize || proceduralModelBase.vertices.X > -1 * minSize) {
		proceduralModelBase.vertices.X = defaultVertices.X;
	}
	if (proceduralModelBase.vertices.Y < minSize || proceduralModelBase.vertices.Y > -1 * minSize) {
		proceduralModelBase.vertices.Y = defaultVertices.Y;
	}
	if (proceduralModelBase.vertices.Z < minSize || proceduralModelBase.vertices.Z > -1 * minSize) {
		proceduralModelBase.vertices.Z = defaultVertices.Z;
	}

	return proceduralModelBase;
}

void PMBase::Create() {
	if (_proceduralModelBase.tagsString.Len() > 0) {
		_proceduralModelBase.tagsString.ParseIntoArray(_proceduralModelBase.tags, TEXT(","), true);
	}
	if (_proceduralModelBase.category == ProceduralModelCategory::CYLINDER) {
		PMCylinder::Create();
	} else if (_proceduralModelBase.category == ProceduralModelCategory::CUBE) {
		PMCube::Create();
	}
}

AStaticMeshActor* PMBase::CreateActor(FString name, FVector location, FRotator rotation,
	FVector scale, FActorSpawnParameters spawnParams, USceneComponent* parent) {
	// AActor* actor = (AActor*)World->SpawnActor<AActor>(AActor::StaticClass(), location * 100,
	// 	rotation, spawnParams);
	AStaticMeshActor* actor = (AStaticMeshActor*)World->SpawnActor<AStaticMeshActor>(
		AStaticMeshActor::StaticClass(), location * 100, rotation, spawnParams);
	actor->SetActorLabel(name);
	if (parent) {
		actor->AttachToComponent(parent, FAttachmentTransformRules::KeepRelativeTransform);
	}
	return actor;
}

UProceduralMeshComponent* PMBase::CreateMesh(UObject* parentObject, USceneComponent* parent, FString name) {
	name = Lodash::GetInstanceId(name + "_");
	UProceduralMeshComponent* ProceduralMesh = NewObject<UProceduralMeshComponent>(parentObject,
		UProceduralMeshComponent::StaticClass(), *name);
    ProceduralMesh->RegisterComponent();
    ProceduralMesh->AttachToComponent(parent, FAttachmentTransformRules::KeepRelativeTransform);
    ProceduralMesh->bUseComplexAsSimpleCollision = true;
	return ProceduralMesh;
}

void PMBase::AddMeshSection(UProceduralMeshComponent* ProceduralMesh, TArray<FVector> Vertices,
	TArray<FVector2D> UV0, TArray<int> Triangles) {
	ProceduralMesh->CreateMeshSection(0, Vertices, Triangles, TArray<FVector>(), UV0,
		TArray<FColor>(), TArray<FProcMeshTangent>(), true);
}
