#include "PMBase.h"

#include "ProceduralMeshComponent.h"
#include "ProceduralMeshConversion.h"
#include "StaticMeshDescription.h"

#include "../Common/Lodash.h"
#include "../Common/UnrealGlobal.h"
#include "../ModelingStructsActor.h"

#include "PMCube.h"
#include "PMCylinder.h"
#include "PMPrism.h"

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

void PMBase::DestroyActors() {
	for (auto& Elem : _spawnedActors) {
		Elem.Value->Destroy();
	}
	_spawnedActors.Empty();
}

void PMBase::CleanUp() {
	DestroyActors();
}

void PMBase::SetInputs(FProceduralModelBase proceduralModelBase) {
	_proceduralModelBase = proceduralModelBase;
}

FProceduralModelBase PMBase::GetInputs(FString defaultName, FVector defaultSize, FVector defaultVertices,
									   int32 defaultSidesSegmentCount, float defaultTopOffsetWidth) {
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
	if (proceduralModelBase.sidesSegmentCount < 3)
	{
		proceduralModelBase.sidesSegmentCount = defaultSidesSegmentCount;
	}
	if (proceduralModelBase.topOffsetWidth < minSize || proceduralModelBase.topOffsetWidth > -1 * minSize)
	{
		proceduralModelBase.topOffsetWidth = defaultTopOffsetWidth;
	}

	return proceduralModelBase;
}

void PMBase::Create() {
	if (_proceduralModelBase.tagsString.Len() > 0) {
		_proceduralModelBase.tagsString.ParseIntoArray(_proceduralModelBase.tags, TEXT(","), true);
	}
	if (_proceduralModelBase.category == ProceduralModelCategory::CYLINDER) {
		PMCylinder::CreateFromInputs();
	} else if (_proceduralModelBase.category == ProceduralModelCategory::CUBE) {
		PMCube::CreateFromInputs();
	}
	else if (_proceduralModelBase.category == ProceduralModelCategory::PRISM)
	{
		PMPrism::CreateFromInputs();
	}
}

AStaticMeshActor* PMBase::CreateActor(FString name, FVector location, FRotator rotation,
	FActorSpawnParameters spawnParams, USceneComponent* parent) {
	UnrealGlobal* unrealGlobal = UnrealGlobal::GetInstance();

	// In case of recompile in editor, will lose reference so need to check scene too.
	AActor* actor1 = unrealGlobal->GetActorByName(name, AStaticMeshActor::StaticClass());
	if (actor1) {
		return (AStaticMeshActor*)actor1;
	}

	spawnParams.Name = FName(name);
	// AActor* actor = (AActor*)World->SpawnActor<AActor>(AActor::StaticClass(), location * unrealGlobal->GetScale(),
	// 	rotation, spawnParams);
	AStaticMeshActor* actor = (AStaticMeshActor*)World->SpawnActor<AStaticMeshActor>(
		AStaticMeshActor::StaticClass(), location * unrealGlobal->GetScale(), rotation, spawnParams);
	_spawnedActors.Add(name, actor);
	unrealGlobal->SetActorFolder(actor);
	actor->SetActorLabel(name);
	if (parent) {
		actor->AttachToComponent(parent, FAttachmentTransformRules::KeepRelativeTransform);
	}
	return actor;
}

void PMBase::AddMesh(AStaticMeshActor* actor, UStaticMesh* mesh, FString materialPath) {
	UStaticMeshComponent* meshComponent = actor->FindComponentByClass<UStaticMeshComponent>();
	meshComponent->SetStaticMesh(mesh);
	if (materialPath.Len() > 0) {
		if (!meshComponent) {
			meshComponent = actor->FindComponentByClass<UStaticMeshComponent>();
		}
		UMaterial* material = Cast<UMaterial>(StaticLoadObject(UMaterial::StaticClass(), NULL,
			*materialPath));
		meshComponent->SetMaterial(0, material);
	}
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

void PMBase::AddMeshSection(UProceduralMeshComponent *ProceduralMesh, TArray<FVector> Vertices,
							TArray<FVector2D> UV0, TArray<int> Triangles, TArray<FVector> Normals, TArray<FProcMeshTangent> Tangents)
{
	ProceduralMesh->CreateMeshSection(0, Vertices, Triangles, Normals, UV0,
									  TArray<FColor>(), Tangents, true);
}

// https://forums.unrealengine.com/t/procedural-mesh-not-saving-all-of-its-sections-to-static-mesh/382319/17
UStaticMesh* PMBase::ToStaticMesh(UProceduralMeshComponent* ProceduralMesh) {
	UStaticMesh* StaticMesh = nullptr;
	if (IsValid(ProceduralMesh) == false) {
		return StaticMesh;
	}
	FName PMCName = FName(*ProceduralMesh->GetName());
	StaticMesh = NewObject<UStaticMesh>(GetTransientPackage(), PMCName, EObjectFlags::RF_Transient);
	StaticMesh->bAllowCPUAccess = true;
	StaticMesh->NeverStream = true;
	StaticMesh->InitResources();
	StaticMesh->SetLightingGuid();

	FMeshDescription PMCDescription = BuildMeshDescription(ProceduralMesh);
	UStaticMeshDescription* SMDescription = StaticMesh->CreateStaticMeshDescription();
	SMDescription->SetMeshDescription(PMCDescription);
	StaticMesh->BuildFromStaticMeshDescriptions({ SMDescription }, false);

	// Collision
	StaticMesh->CalculateExtendedBounds();
	StaticMesh->SetBodySetup(ProceduralMesh->ProcMeshBodySetup);

#if WITH_EDITOR    
	StaticMesh->PostEditChange();
#endif

	StaticMesh->MarkPackageDirty();
	return StaticMesh;
}
