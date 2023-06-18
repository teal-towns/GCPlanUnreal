#include "PMBase.h"

#include "ProceduralMeshComponent.h"
#include "ProceduralMeshConversion.h"
#include "StaticMeshDescription.h"

#include "../Common/Lodash.h"
#include "../Common/UnrealGlobal.h"
#include "../Mesh/LoadContent.h"
#include "../Modeling/ModelBase.h"
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

void PMBase::CleanUp() {
}

void PMBase::SetInputs(FProceduralModelBase proceduralModelBase) {
	_proceduralModelBase = proceduralModelBase;
}

std::tuple<FProceduralModelBase, FModelParams> PMBase::GetInputs(FString defaultName, FVector defaultSize,
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

	// if (proceduralModelBase.vertices.X < minSize || proceduralModelBase.vertices.X > -1 * minSize) {
	// 	proceduralModelBase.vertices.X = defaultVertices.X;
	// }
	// if (proceduralModelBase.vertices.Y < minSize || proceduralModelBase.vertices.Y > -1 * minSize) {
	// 	proceduralModelBase.vertices.Y = defaultVertices.Y;
	// }
	// if (proceduralModelBase.vertices.Z < minSize || proceduralModelBase.vertices.Z > -1 * minSize) {
	// 	proceduralModelBase.vertices.Z = defaultVertices.Z;
	// }
	if (proceduralModelBase.sidesSegmentCount < 3)
	{
		proceduralModelBase.sidesSegmentCount = defaultSidesSegmentCount;
	}
	if (proceduralModelBase.topOffsetWidth < minSize || proceduralModelBase.topOffsetWidth > -1 * minSize)
	{
		proceduralModelBase.topOffsetWidth = defaultTopOffsetWidth;
	}

	FModelParams modelParams;
	if (proceduralModelBase.materialKey.Len() > 0) {
		LoadContent* loadContent = LoadContent::GetInstance();
		modelParams.materialPath = loadContent->Material(proceduralModelBase.materialKey);
	}

	return { proceduralModelBase, modelParams };
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

UProceduralMeshComponent* PMBase::CreateMesh(FString name, UObject* parentObject, USceneComponent* parent) {
	name = Lodash::GetInstanceId(name + "_");
	UProceduralMeshComponent* ProceduralMesh = NewObject<UProceduralMeshComponent>(parentObject,
		UProceduralMeshComponent::StaticClass(), *name);
    ProceduralMesh->CreationMethod = EComponentCreationMethod::Instance;
    ProceduralMesh->RegisterComponent();
    if (parent) {
    	ProceduralMesh->AttachToComponent(parent, FAttachmentTransformRules::KeepRelativeTransform);
    }
    ProceduralMesh->bUseComplexAsSimpleCollision = true;
	return ProceduralMesh;
}

std::tuple<UProceduralMeshComponent*, AActor*> PMBase::GetMesh() {
	ModelBase* modelBase = ModelBase::GetInstance();
	FString name = Lodash::GetInstanceId("PMGetMesh_");
	AStaticMeshActor* actor = modelBase->CreateActor(name);
	UObject* parentObject = (UObject*)actor;
	UProceduralMeshComponent* proceduralMesh = PMBase::CreateMesh(name + "_Mesh", parentObject);
	return { proceduralMesh, actor };
}

void PMBase::DestroyMesh(AActor* actor, UProceduralMeshComponent* proceduralMesh) {
	if (IsValid(actor)) {
		actor->Destroy();
	}
}

void PMBase::AddMeshSection(UProceduralMeshComponent* proceduralMesh, TArray<FVector> Vertices,
	TArray<FVector2D> UV0, TArray<int> Triangles, TArray<FVector> Normals,
	TArray<FProcMeshTangent> Tangents, FModelParams modelParams) {
	proceduralMesh->CreateMeshSection(0, Vertices, Triangles, Normals, UV0,
		TArray<FColor>(), Tangents, true);
	if (modelParams.dynamicMaterial) {
		proceduralMesh->SetMaterial(0, modelParams.dynamicMaterial);
	} else if (modelParams.materialPath.Len() > 0) {
		if (modelParams.materialPath.Contains(".MaterialInstance")) {
			UMaterialInstance* material = Cast<UMaterialInstance>(StaticLoadObject(UMaterialInstance::StaticClass(),
				NULL, *modelParams.materialPath));
			proceduralMesh->SetMaterial(0, material);
		} else {
			UMaterial* material = Cast<UMaterial>(StaticLoadObject(UMaterial::StaticClass(), NULL,
				*modelParams.materialPath));
			proceduralMesh->SetMaterial(0, material);
		}
	}
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

AStaticMeshActor* PMBase::MeshToActor(FString name, UProceduralMeshComponent* proceduralMesh,
	FModelCreateParams createParams, FModelParams modelParams) {
	ModelBase* modelBase = ModelBase::GetInstance();
	UStaticMesh* mesh = ToStaticMesh(proceduralMesh);
	// AddMesh(createParams.parentActor, mesh);
	FVector rotation = FVector(0,0,0);
	FActorSpawnParameters spawnParams;
	FVector location = FVector(0,0,0);
	FVector scale = FVector(1,1,1);
	modelParams.mesh = mesh;
	modelParams.parent = createParams.parent;
	return modelBase->CreateActor(name, location, rotation, scale, spawnParams, modelParams);
}
