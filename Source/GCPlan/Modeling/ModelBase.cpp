#include "ModelBase.h"

#include "Engine/StaticMeshActor.h"
#include "Components/StaticMeshComponent.h"

#include "../Common/Lodash.h"
#include "../Common/UnrealGlobal.h"
#include "../ModelingStructsActor.h"
#include "ModelBench.h"
#include "ModelDesk.h"
#include "ModelStreetLight.h"
#include "ModelEVCharger.h"

ModelBase* ModelBase::pinstance_{nullptr};
std::mutex ModelBase::mutex_;

ModelBase::ModelBase() {
}

ModelBase::~ModelBase() {
}

ModelBase *ModelBase::GetInstance() {
	std::lock_guard<std::mutex> lock(mutex_);
	if (pinstance_ == nullptr) {
		pinstance_ = new ModelBase();
	}
	return pinstance_;
}

void ModelBase::SetWorld(UWorld* World1) {
	World = World1;
}

UWorld* ModelBase::GetWorld() {
	return World;
}

void ModelBase::DestroyActors() {
	for (auto& Elem : _spawnedActors) {
		Elem.Value->Destroy();
	}
	_spawnedActors.Empty();
}

void ModelBase::CleanUp() {
	DestroyActors();
}

void ModelBase::SetInputs(FModelingBase modelingBase) {
	_modelingBase = modelingBase;
}

FModelingBase ModelBase::GetInputs(FString defaultName, FVector defaultSize) {
	FModelingBase modelingBase = _modelingBase;
	if (modelingBase.name.Len() < 1) {
		modelingBase.name = defaultName;
	}
	modelingBase.name = Lodash::GetInstanceId(modelingBase.name + "_");

	float minSize = 0.01;
	if (modelingBase.size.X < minSize || modelingBase.size.X > -1 * minSize) {
		modelingBase.size.X = defaultSize.X;
	}
	if (modelingBase.size.Y < minSize || modelingBase.size.Y > -1 * minSize) {
		modelingBase.size.Y = defaultSize.Y;
	}
	if (modelingBase.size.Z < minSize || modelingBase.size.Z > -1 * minSize) {
		modelingBase.size.Z = defaultSize.Z;
	}

	return modelingBase;
}

void ModelBase::Create() { 
	FVector defLocation = FVector(0, 0, 0);

	if (_modelingBase.tagsString.Len() > 0) {
		_modelingBase.tagsString.ParseIntoArray(_modelingBase.tags, TEXT(","), true);
	}

	if (_modelingBase.category == ModelingCategory::ANIMAL) {
		_modelingBase.subCategory = *UEnum::GetValueAsString(_modelingBase.subCategoryAnimal);
	} else if (_modelingBase.category == ModelingCategory::ARCHITECTURE) {
		_modelingBase.subCategory = *UEnum::GetValueAsString(_modelingBase.subCategoryArchitecture);
	} else if (_modelingBase.category == ModelingCategory::CLOTHES) {
		_modelingBase.subCategory = *UEnum::GetValueAsString(_modelingBase.subCategoryClothes);
	} else if (_modelingBase.category == ModelingCategory::ELECTRONICS) {
		_modelingBase.subCategory = *UEnum::GetValueAsString(_modelingBase.subCategoryElectronics);
	} else if (_modelingBase.category == ModelingCategory::FOOD) {
		_modelingBase.subCategory = *UEnum::GetValueAsString(_modelingBase.subCategoryFood);
	} else if (_modelingBase.category == ModelingCategory::FURNITURE) {
		_modelingBase.subCategory = *UEnum::GetValueAsString(_modelingBase.subCategoryFurniture);
	} else if (_modelingBase.category == ModelingCategory::PLANTS) {
		_modelingBase.subCategory = *UEnum::GetValueAsString(_modelingBase.subCategoryPlants);
	} else if (_modelingBase.category == ModelingCategory::SPORTS) {
		_modelingBase.subCategory = *UEnum::GetValueAsString(_modelingBase.subCategorySports);
	} else if (_modelingBase.category == ModelingCategory::TOOLS) {
		_modelingBase.subCategory = *UEnum::GetValueAsString(_modelingBase.subCategoryTools);
	} else if (_modelingBase.category == ModelingCategory::VEHICLE) {
		_modelingBase.subCategory = *UEnum::GetValueAsString(_modelingBase.subCategoryVehicle);
	}

	if (_modelingBase.subCategory == "BENCH") {
		ModelBench::Create();
	} else if (_modelingBase.subCategory == "DESK") {
		ModelDesk::Create();
	} else if (_modelingBase.subCategory == "MONITOR") {
		ModelMonitor::Create(defLocation);
	} else if (_modelingBase.subCategory == "KEYBOARD") {
		ModelKeyboard::Create(defLocation);
	} else if (_modelingBase.subCategory == "MOUSE") {
		ModelMouse::Create(defLocation);
	} else if (_modelingBase.subCategory == "STREETLIGHT") {
		ModelStreetLight::Create();
	} else if (_modelingBase.subCategory == "EVCHARGER") {
		ModelEVCharger::Create();
	}
}

AStaticMeshActor* ModelBase::CreateActor(FString name, FVector location, FRotator rotation,
	FVector scale, FActorSpawnParameters spawnParams, USceneComponent* parent, FString meshPath,
	FString materialPath, UStaticMesh* mesh) {
	UnrealGlobal* unrealGlobal = UnrealGlobal::GetInstance();

	// In case of recompile in editor, will lose reference so need to check scene too.
	AActor* actor1 = unrealGlobal->GetActorByName(name, AStaticMeshActor::StaticClass());
	if (actor1) {
		return (AStaticMeshActor*)actor1;
	}

	spawnParams.Name = FName(name);
	AStaticMeshActor* actor = (AStaticMeshActor*)World->SpawnActor<AStaticMeshActor>(
		AStaticMeshActor::StaticClass(), location * unrealGlobal->GetScale(), rotation, spawnParams);
	_spawnedActors.Add(name, actor);
	unrealGlobal->SetActorFolder(actor);
	actor->SetActorLabel(name);
	if (scale.X != 1 || scale.Y != 1 || scale.Z != 1) {
		actor->SetActorScale3D(scale);
	}
	UStaticMeshComponent* meshComponent = nullptr;
	if (mesh) {
		if (!meshComponent) {
			meshComponent = actor->FindComponentByClass<UStaticMeshComponent>();
		}
		meshComponent->SetStaticMesh(mesh);
	} else if (meshPath.Len() > 0) {
		if (!meshComponent) {
			meshComponent = actor->FindComponentByClass<UStaticMeshComponent>();
		}
		// meshComponent->SetStaticMesh(meshPath);
		// const TCHAR* path = *meshPath;
		// UStaticMesh* mesh = LoadObject(nullptr, *meshPath);
		// UStaticMesh* mesh = LoadObject(nullptr, "/Script/Engine.StaticMesh'/Game/Modeling/Primitives/Cube.Cube'");
		// UStaticMesh* mesh = Cast<UStaticMesh>(StaticLoadObject(UStaticMesh::StaticClass(), NULL,
		// 	TEXT("/Script/Engine.StaticMesh'/Game/Modeling/Primitives/Cube.Cube'")));
		mesh = Cast<UStaticMesh>(StaticLoadObject(UStaticMesh::StaticClass(), NULL,
			*meshPath));
		meshComponent->SetStaticMesh(mesh);
		// const ConstructorHelpers::FObjectFinder<UStaticMesh> MeshObj(TEXT("/Script/Engine.StaticMesh'/Game/Modeling/Primitives/Cube.Cube'"));
		// meshComponent->SetStaticMesh(MeshObj.Object);
	}
	if (materialPath.Len() > 0) {
		if (!meshComponent) {
			meshComponent = actor->FindComponentByClass<UStaticMeshComponent>();
		}
		UMaterial* material = Cast<UMaterial>(StaticLoadObject(UMaterial::StaticClass(), NULL,
			*materialPath));
		meshComponent->SetMaterial(0, material);
	}
	if (parent) {
		actor->AttachToComponent(parent, FAttachmentTransformRules::KeepRelativeTransform);
	}
	return actor;
}
