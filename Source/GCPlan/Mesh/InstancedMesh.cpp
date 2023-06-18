#include "InstancedMesh.h"

#include "Engine/StaticMeshActor.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Components/StaticMeshComponent.h"

#include "../Common/UnrealGlobal.h"
#include "../Mesh/LoadContent.h"
#include "../ProceduralModel/PMBase.h"
#include "../Modeling/ModelBase.h"

InstancedMesh* InstancedMesh::pinstance_{nullptr};
std::mutex InstancedMesh::mutex_;

InstancedMesh::InstancedMesh() {
}

InstancedMesh::~InstancedMesh() {
}

InstancedMesh *InstancedMesh::GetInstance() {
	std::lock_guard<std::mutex> lock(mutex_);
	if (pinstance_ == nullptr) {
		pinstance_ = new InstancedMesh();
	}
	return pinstance_;
}

void InstancedMesh::SetWorld(UWorld* World1) {
	World = World1;
}

void InstancedMesh::CleanUp() {
	for (auto& Elem : _instancedMeshActors) {
		ClearInstances(Elem.Key);
		if (IsValid(Elem.Value)) {
			Elem.Value->Destroy();
		}
	}
	_instancedMeshActors = {};
	if (_instancedMeshesActor && IsValid(_instancedMeshesActor)) {
		_instancedMeshesActor->Destroy();
		// _instancedMeshesActor = nullptr;
	}
	pinstance_ = nullptr;
}

void InstancedMesh::InitMeshes() {
	if (!IsValid(_instancedMeshesActor)) {
		FString name = "InstancedMeshes";
		ModelBase* modelBase = ModelBase::GetInstance();
		PMBase* pmBase = PMBase::GetInstance();
		FActorSpawnParameters spawnParams;
		_instancedMeshesActor = modelBase->CreateActor(name, FVector(0,0,0), FRotator(0,0,0),
			FVector(1,1,1), spawnParams);
	} else {
		UE_LOG(LogTemp, Display, TEXT("InitMeshes actor still valid"));
		// if (_instancedMeshesActor) {
		// 	UE_LOG(LogTemp, Display, TEXT("pending?"), _instancedMeshesActor->IsPendingKill(), _instancedMeshesActor->HasAnyFlags(RF_BeginDestroyed));
		// }
	}

	// LoadContent* loadContent = LoadContent::GetInstance();
	// TODO - replace and / or reference these all from load content
	AddMesh("RoadRoundabout", "/Script/Engine.StaticMesh'/Game/Landscape/Roundabout1.Roundabout1'",
		"/Script/Engine.Material'/Game/Landscape/Asphalt_M.Asphalt_M'");
	AddMesh("VertexWhite", "/Script/Engine.StaticMesh'/Game/Landscape/VerticesEdit/VertexWhite.VertexWhite'",
		"");
	AddMesh("EdgeBlack", "/Script/Engine.StaticMesh'/Game/Landscape/VerticesEdit/EdgeBlack.EdgeBlack'",
		"");
	AddMesh("EdgeBlue", "/Script/Engine.StaticMesh'/Game/Landscape/VerticesEdit/EdgeBlue.EdgeBlue'",
		"");
	AddMesh("EdgeRed", "/Script/Engine.StaticMesh'/Game/Landscape/VerticesEdit/EdgeRed.EdgeRed'",
		"");
}

// UWorld* InstancedMesh::GetWorld() {
// 	return World;
// }

void InstancedMesh::AddMesh(FString name, FString meshPath, FString materialPath) {
	if (!_instancedMeshActors.Contains(name)) {
		AActor* actor;
		// If recompile in editor, will not exist in memory, so need to check scene too.
		UnrealGlobal* unrealGlobal = UnrealGlobal::GetInstance();
		actor = unrealGlobal->GetActorByName(name, AActor::StaticClass());
		if (actor) {
			_instancedMeshActors.Add(name, actor);
		} else {
			// PMBase* pmBase = PMBase::GetInstance();
			// ModelBase* modelBase = ModelBase::GetInstance();
			USceneComponent* meshesParent = _instancedMeshesActor->FindComponentByClass<USceneComponent>();
			FActorSpawnParameters spawnParams;
			spawnParams.Name = FName(name);
			actor = (AActor*)World->SpawnActor<AActor>(
				AActor::StaticClass(), FVector(0,0,0) * unrealGlobal->GetScale(), FRotator(0,0,0), spawnParams);
			actor->SetActorLabel(name);
			_instancedMeshActors.Add(name, actor);

			USceneComponent* parent = actor->FindComponentByClass<USceneComponent>();
			UObject* parentObject = (UObject*)actor;
			FString nameTemp = name + "_ISM";
			UInstancedStaticMeshComponent* instancedStaticMesh = NewObject<UInstancedStaticMeshComponent>(parentObject,
				UInstancedStaticMeshComponent::StaticClass(), *nameTemp);
			instancedStaticMesh->RegisterComponent();
			// instancedStaticMesh->AttachToComponent(parent, FAttachmentTransformRules::KeepRelativeTransform);
			actor->SetRootComponent(instancedStaticMesh);
			// Must be after has root component.
			actor->AttachToComponent(meshesParent, FAttachmentTransformRules::KeepRelativeTransform);
			instancedStaticMesh->SetMobility(EComponentMobility::Static);

			UStaticMesh* mesh = Cast<UStaticMesh>(StaticLoadObject(UStaticMesh::StaticClass(), NULL,
				*meshPath));
			// UStaticMeshComponent* meshComponent = actor->FindComponentByClass<UStaticMeshComponent>();
			// meshComponent->SetStaticMesh(mesh);
			instancedStaticMesh->SetStaticMesh(mesh);
			UMaterial* material = Cast<UMaterial>(StaticLoadObject(UMaterial::StaticClass(), NULL,
				*materialPath));
			instancedStaticMesh->SetMaterial(0, material);
		}
	} else {
		// return _instancedMeshActors[name];
	}

	// return actor;
}

int InstancedMesh::CreateInstance(FString meshKey, FVector Translation, FRotator Rotation, FVector Scale,
	bool unrealScaleTranslation) {
	int index = -1;
	if (_instancedMeshActors.Contains(meshKey)) {
		if (unrealScaleTranslation) {
			Translation = UnrealGlobal::Scale(Translation);
		}
		UInstancedStaticMeshComponent* component = _instancedMeshActors[meshKey]->FindComponentByClass<UInstancedStaticMeshComponent>();
		index = component->AddInstance(FTransform(Rotation, Translation, Scale));
	}
	return index;
}

void InstancedMesh::ClearInstances(FString meshKey) {
	if (_instancedMeshActors.Contains(meshKey)) {
		UInstancedStaticMeshComponent* component = _instancedMeshActors[meshKey]->FindComponentByClass<UInstancedStaticMeshComponent>();
		component->ClearInstances();
	}
}

int InstancedMesh::UpdateInstance(FString meshKey, int instanceIndex, FVector Translation, FRotator Rotation,
	FVector Scale, bool unrealScaleTranslation) {
	int index = -1;
	if (_instancedMeshActors.Contains(meshKey)) {
		UInstancedStaticMeshComponent* component = _instancedMeshActors[meshKey]->FindComponentByClass<UInstancedStaticMeshComponent>();
		if (instanceIndex < component->GetInstanceCount()) {
			if (unrealScaleTranslation) {
				Translation = UnrealGlobal::Scale(Translation);
			}
			bool valid = component->UpdateInstanceTransform(instanceIndex, FTransform(Rotation, Translation, Scale), false, true, true);
			if (valid) {
				index = instanceIndex;
			}
		}
	}
	return index;
}

int InstancedMesh::SaveInstance(FString meshKey, int instanceIndex, FVector Translation, FRotator Rotation,
	FVector Scale, bool unrealScaleTranslation) {
	if (instanceIndex < 0) {
		return CreateInstance(meshKey, Translation, Rotation, Scale, unrealScaleTranslation);
	}
	return UpdateInstance(meshKey, instanceIndex, Translation, Rotation, Scale, unrealScaleTranslation);
}

bool InstancedMesh::RemoveInstances(FString meshKey, TArray<int> instanceIndices) {
	if (_instancedMeshActors.Contains(meshKey) && instanceIndices.Num() > 0) {
		UInstancedStaticMeshComponent* component = _instancedMeshActors[meshKey]->FindComponentByClass<UInstancedStaticMeshComponent>();
		return component->RemoveInstances(instanceIndices);
	}
	return false;
}

void InstancedMesh::ClearInstancesBulk(TArray<FString> meshKeys) {
	for (int ii = 0; ii < meshKeys.Num(); ii++) {
		ClearInstances(meshKeys[ii]);
	}
}
