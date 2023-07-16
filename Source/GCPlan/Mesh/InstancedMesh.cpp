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
	}
	_instancedMeshesActor = nullptr;
	pinstance_ = nullptr;
}

void InstancedMesh::Init() {
	if (!_instancedMeshesActor || !IsValid(_instancedMeshesActor)) {
		FString name = "InstancedMeshes";
		ModelBase* modelBase = ModelBase::GetInstance();
		PMBase* pmBase = PMBase::GetInstance();
		FActorSpawnParameters spawnParams;
		_instancedMeshesActor = modelBase->CreateActor(name, FVector(0,0,0), FVector(0,0,0),
			FVector(1,1,1), spawnParams);
	} else {
		UE_LOG(LogTemp, Display, TEXT("InitMeshes actor still valid"));
		// if (_instancedMeshesActor) {
		// 	UE_LOG(LogTemp, Display, TEXT("pending?"), _instancedMeshesActor->IsPendingKill(), _instancedMeshesActor->HasAnyFlags(RF_BeginDestroyed));
		// }
	}
}

void InstancedMesh::InitMeshes() {
	Init();

	// LoadContent* loadContent = LoadContent::GetInstance();
	// TODO - replace and / or reference these all from load content
	AddMesh("RoadRoundabout", "/Script/Engine.StaticMesh'/Game/Landscape/Roundabout1.Roundabout1'",
		"/Script/Engine.MaterialInstanceConstant'/Game/Landscape/Spline_M_Inst_Asphalt.Spline_M_Inst_Asphalt'");
	AddMesh("VertexWhite", "/Script/Engine.StaticMesh'/Game/Landscape/VerticesEdit/VertexWhite.VertexWhite'",
		"");
	AddMesh("VertexBlack", "/Script/Engine.StaticMesh'/Game/Landscape/VerticesEdit/VertexBlack.VertexBlack'",
		"");
	AddMesh("VertexGrey", "/Script/Engine.StaticMesh'/Game/Landscape/VerticesEdit/VertexGrey.VertexGrey'",
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

// bool InstancedMesh::MeshExists(FString name) {
// 	if (_instancedMeshActors.Contains(name)) {
// 		return true;
// 	}
// 	return false;
// }

void InstancedMesh::AddMesh(FString name, FString meshPath, FString materialPath, FModelParams modelParams) {
	if (!_instancedMeshActors.Contains(name)) {
		AActor* actor;
		// If recompile in editor, will not exist in memory, so need to check scene too.
		UnrealGlobal* unrealGlobal = UnrealGlobal::GetInstance();
		actor = unrealGlobal->GetActorByName(name, AActor::StaticClass());
		if (actor) {
			_instancedMeshActors.Add(name, actor);
		} else {
			UStaticMesh* mesh;
			bool meshValid = false;
			LoadContent* loadContent = LoadContent::GetInstance();
			if (modelParams.mesh) {
				mesh = modelParams.mesh;
				meshValid = true;
			} else {
				if (meshPath.Len() < 1) {
					if (modelParams.meshPath.Len() < 1 && modelParams.meshKey.Len() > 0) {
						modelParams.meshPath = loadContent->Mesh(modelParams.meshKey);
					}
					meshPath = modelParams.meshPath;
				}
				mesh = Cast<UStaticMesh>(StaticLoadObject(UStaticMesh::StaticClass(), NULL, *meshPath));
				if (!mesh) {
					UE_LOG(LogTemp, Warning, TEXT("InstancedMesh.AddMesh mesh load error, skipping %s"), *meshPath);
				} else {
					meshValid = true;
				}
			}

			if (meshValid) {
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
				instancedStaticMesh->CreationMethod = EComponentCreationMethod::Instance;
				instancedStaticMesh->RegisterComponent();
				// instancedStaticMesh->AttachToComponent(parent, FAttachmentTransformRules::KeepRelativeTransform);
				actor->SetRootComponent(instancedStaticMesh);
				// Must be after has root component.
				actor->AttachToComponent(meshesParent, FAttachmentTransformRules::KeepRelativeTransform);
				instancedStaticMesh->SetMobility(EComponentMobility::Static);

				instancedStaticMesh->SetStaticMesh(mesh);

				if (modelParams.dynamicMaterial) {
					instancedStaticMesh->SetMaterial(0, modelParams.dynamicMaterial);
				} else {
					if (materialPath.Len() < 1) {
						if (modelParams.materialPath.Len() < 1 && modelParams.materialKey.Len() > 0) {
							modelParams.materialPath = loadContent->Material(modelParams.materialKey);
						}
						materialPath = modelParams.materialPath;
					}
					if (materialPath.Len() > 0) {
						if (materialPath.Contains(".MaterialInstance")) {
							UMaterialInstance* material = Cast<UMaterialInstance>(StaticLoadObject(UMaterialInstance::StaticClass(), NULL,
								*materialPath));
							if (!material) {
								UE_LOG(LogTemp, Warning, TEXT("InstancedMesh.AddMesh material load error, skipping %s"), *materialPath);
							} else {
								instancedStaticMesh->SetMaterial(0, material);
							}
						} else {
							UMaterial* material = Cast<UMaterial>(StaticLoadObject(UMaterial::StaticClass(), NULL,
								*materialPath));
							if (!material) {
								UE_LOG(LogTemp, Warning, TEXT("InstancedMesh.AddMesh material load error, skipping %s"), *materialPath);
							} else {
								instancedStaticMesh->SetMaterial(0, material);
							}
						}
					}
				}
			}
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
