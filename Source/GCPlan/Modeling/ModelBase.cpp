#include "ModelBase.h"

#include "Engine/StaticMeshActor.h"
#include "Components/StaticMeshComponent.h"

#include "../Common/Lodash.h"
#include "../Common/UnrealGlobal.h"
#include "../Mesh/LoadContent.h"
#include "../ModelingStructsActor.h"
#include "Electronics/ModelComputer.h"
#include "Electronics/ModelMonitor.h"
#include "Furniture/ModelBook.h"
#include "Furniture/ModelBench.h"
#include "Furniture/ModelChair.h"
#include "Furniture/ModelCouch.h"
#include "Furniture/ModelDesk.h"
#include "Furniture/ModelLight.h"
#include "Furniture/ModelStand.h"
#include "Furniture/ModelTable.h"
#include "Plants/ModelBush.h"
#include "Plants/ModelTree.h"
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
		if (IsValid(Elem.Value)) {
			Elem.Value->Destroy();
			// UE_LOG(LogTemp, Display, TEXT("destroying %s"), *Elem.Key);
		} else {
			UE_LOG(LogTemp, Display, TEXT("NOT destroying %s"), *Elem.Key);
		}
	}
	_spawnedActors.Empty();
}

void ModelBase::CleanUp() {
	DestroyActors();
}

void ModelBase::SetInputs(FModelingBase modelingBase) {
	_modelingBase = modelingBase;
}

std::tuple<FModelingBase, FModelParams> ModelBase::GetInputs(FString defaultName, FVector defaultSize) {
	FModelingBase modelingBase = _modelingBase;
	if (modelingBase.name.Len() < 1) {
		modelingBase.name = defaultName;
	}
	modelingBase.name = Lodash::GetInstanceId(modelingBase.name + "_");

	float minSize = 0.01;
	if (modelingBase.size.X < minSize) {
		modelingBase.size.X = defaultSize.X;
	}
	if (modelingBase.size.Y < minSize) {
		modelingBase.size.Y = defaultSize.Y;
	}
	if (modelingBase.size.Z < minSize) {
		modelingBase.size.Z = defaultSize.Z;
	}

	FModelParams modelParams;
	if (modelingBase.materialKey.Len() > 0) {
		LoadContent* loadContent = LoadContent::GetInstance();
		modelParams.materialPath = loadContent->Material(modelingBase.materialKey);
	}

	return { modelingBase, modelParams };
}

void ModelBase::Create() { 
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
		ModelBench::CreateFromInputs();
	} else if (_modelingBase.subCategory == "BOOK") {
		ModelBook::CreateFromInputs();
	} else if (_modelingBase.subCategory == "BUSH") {
		ModelBush::CreateFromInputs();
	} else if (_modelingBase.subCategory == "CHAIR") {
		ModelChair::CreateFromInputs();
	} else if (_modelingBase.subCategory == "COUCH") {
		ModelCouch::CreateFromInputs();
	} else if (_modelingBase.subCategory == "DESK") {
		ModelDesk::CreateFromInputs();
	} else if (_modelingBase.subCategory == "EVCHARGER") {
		ModelEVCharger::Create();
	// } else if (_modelingBase.subCategory == "KEYBOARD") {
	// 	ModelComputer::Keyboard();
	} else if (_modelingBase.subCategory == "LIGHT") {
		ModelLight::CreateFromInputs();
	} else if (_modelingBase.subCategory == "MONITOR") {
		ModelMonitor::CreateFromInputs();
	// } else if (_modelingBase.subCategory == "MOUSE") {
	// 	ModelComputer::Mouse();
	} else if (_modelingBase.subCategory == "STAND") {
		ModelStand::CreateFromInputs();
	} else if (_modelingBase.subCategory == "STREETLIGHT") {
		ModelStreetLight::Create();
	} else if (_modelingBase.subCategory == "TABLE") {
		ModelTable::CreateFromInputs();
	} else if (_modelingBase.subCategory == "TREE") {
		ModelTree::CreateFromInputs();
	}
}

void ModelBase::CreateFloor() {
	LoadContent* loadContent = LoadContent::GetInstance();
	FString meshCube = loadContent->Mesh("cube");
	FModelParams modelParams;
	modelParams.meshPath = meshCube;
	modelParams.materialKey = "marbleTile";
	CreateActor(Lodash::GetInstanceId("Floor_"), FVector(0,0,-1), FVector(0,0,0), FVector(10,10,1),
		FActorSpawnParameters(), modelParams);
}

FString ModelBase::CheckGetName(FString name, FString defaultName) {
	if (name == "") {
		return Lodash::GetInstanceId(defaultName);
	}
	return name;
}

AStaticMeshActor* ModelBase::CreateActorEmpty(FString name, FModelParams modelParams) {
	UnrealGlobal* unrealGlobal = UnrealGlobal::GetInstance();
	// In case of recompile in editor, will lose reference so need to check scene too.
	AActor* actor1 = unrealGlobal->GetActorByName(name, AStaticMeshActor::StaticClass());
	if (actor1) {
		return (AStaticMeshActor*)actor1;
	}
	FActorSpawnParameters spawnParams;
	AStaticMeshActor* actor = (AStaticMeshActor*)World->SpawnActor<AStaticMeshActor>(
		AStaticMeshActor::StaticClass(), FVector(0,0,0), FRotator(0,0,0), spawnParams);
	_spawnedActors.Add(name, actor);
	unrealGlobal->SetActorFolder(actor);
	actor->SetActorLabel(name);
	if (modelParams.parent) {
		actor->AttachToComponent(modelParams.parent, FAttachmentTransformRules::KeepRelativeTransform);
	}
	return actor;
}

AStaticMeshActor* ModelBase::CreateActor(FString name, FVector location, FVector rotation,
	FVector scale, FActorSpawnParameters spawnParams, FModelParams modelParams) {
	UnrealGlobal* unrealGlobal = UnrealGlobal::GetInstance();
	if (name == "") {
		name = Lodash::GetInstanceId();
	}

	// In case of recompile in editor, will lose reference so need to check scene too.
	AActor* actor1 = unrealGlobal->GetActorByName(name, AStaticMeshActor::StaticClass());
	if (actor1) {
		return (AStaticMeshActor*)actor1;
	}

	FRotator rotator = FRotator(0,0,0);
	if (rotation.X != 0 || rotation.Y != 0 || rotation.Z != 0) {
		rotator = FRotator(rotation.Y, rotation.Z, rotation.X);
	} else if (modelParams.rotation.X != 0 || modelParams.rotation.Y != 0 || modelParams.rotation.Z != 0) {
		rotator = FRotator(modelParams.rotation.Y, modelParams.rotation.Z, modelParams.rotation.X);
	}
	spawnParams.Name = FName(name);
	if (location.X == 0 && location.Y == 0 && location.Z == 0 && (modelParams.location.X != 0 ||
		modelParams.location.Y != 0 || modelParams.location.Z != 0)) {
		location = modelParams.location;
	}
	AStaticMeshActor* actor = (AStaticMeshActor*)World->SpawnActor<AStaticMeshActor>(
		AStaticMeshActor::StaticClass(), location * unrealGlobal->GetScale(), rotator, spawnParams);
	_spawnedActors.Add(name, actor);
	unrealGlobal->SetActorFolder(actor);
	actor->SetActorLabel(name);
	if (scale.X != 1 || scale.Y != 1 || scale.Z != 1) {
		actor->SetActorScale3D(scale);
	}

	LoadContent* loadContent = LoadContent::GetInstance();
	UStaticMeshComponent* meshComponent = actor->FindComponentByClass<UStaticMeshComponent>();
	SetMeshMaterialFromParams(meshComponent, modelParams);

	if (modelParams.parent) {
		actor->AttachToComponent(modelParams.parent, FAttachmentTransformRules::KeepRelativeTransform);
	}
	return actor;
}

void ModelBase::SetMeshMaterialFromParams(UStaticMeshComponent* meshComponent, FModelParams modelParams) {
	LoadContent* loadContent = LoadContent::GetInstance();
	if (modelParams.meshPath.Len() < 1 && modelParams.meshKey.Len() > 0) {
		modelParams.meshPath = loadContent->Mesh(modelParams.meshKey);
	}
	if (modelParams.mesh) {
		meshComponent->SetStaticMesh(modelParams.mesh);
	} else if (modelParams.meshPath.Len() > 0) {
		UStaticMesh* mesh = Cast<UStaticMesh>(StaticLoadObject(UStaticMesh::StaticClass(), NULL,
			*modelParams.meshPath));
		meshComponent->SetStaticMesh(mesh);
	}

	if (modelParams.materialPath.Len() < 1 && modelParams.materialKey.Len() > 0) {
		modelParams.materialPath = loadContent->Material(modelParams.materialKey);
	}
	if (modelParams.dynamicMaterial) {
		meshComponent->SetMaterial(0, modelParams.dynamicMaterial);
	} else if (modelParams.materialPath.Len() > 0) {
		if (modelParams.materialPath.Contains(".MaterialInstance")) {
			UMaterialInstance* material = Cast<UMaterialInstance>(StaticLoadObject(UMaterialInstance::StaticClass(), NULL,
				*modelParams.materialPath));
			meshComponent->SetMaterial(0, material);
		} else {
			UMaterial* material = Cast<UMaterial>(StaticLoadObject(UMaterial::StaticClass(), NULL,
				*modelParams.materialPath));
			meshComponent->SetMaterial(0, material);
		}
	}
}

void ModelBase::SetTransformFromParams(AActor* actor, FModelCreateParams createParams) {
	SetTransform(actor, createParams.offset, createParams.rotation);
}

void ModelBase::SetTransform(AActor* actor, FVector location, FVector rotation, FVector scale) {
	// Order matters? Do rotation first.
	if (rotation.X != 0 || rotation.Y != 0 || rotation.Z != 0) {
		actor->SetActorRotation(FRotator(rotation.Y, rotation.Z, rotation.X));
	}
	if (scale.X != 1 || scale.Y != 1 || scale.Z != 1) {
		actor->SetActorScale3D(scale);
	}
	if (location.X != 0 || location.Y != 0 || location.Z != 0) {
		UnrealGlobal* unrealGlobal = UnrealGlobal::GetInstance();
		actor->SetActorLocation(location * unrealGlobal->GetScale());
	}
}
