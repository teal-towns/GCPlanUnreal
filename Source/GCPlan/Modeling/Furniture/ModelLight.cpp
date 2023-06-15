#include "ModelLight.h"

#include "Engine/StaticMeshActor.h"

#include "../ModelBase.h"
#include "../../Mesh/LoadContent.h"
#include "../../ModelingStructsActor.h"
#include "../../ProceduralModel/PMCylinder.h"

ModelLight::ModelLight() {
}

ModelLight::~ModelLight() {
}

void ModelLight::Create() {
	ModelBase* modelBase = ModelBase::GetInstance();
	UWorld* World = modelBase->GetWorld();
	FModelingBase modelingBase = modelBase->GetInputs("Light1", FVector(3,1,1));
	FString name = modelingBase.name;
	FVector size = modelingBase.size;
	TArray<FString> tags = modelingBase.tags;

	FRotator rotation = FRotator(0,0,0);
	FActorSpawnParameters spawnParams;
	FVector location = FVector(0,0,0);
	FVector scale = FVector(1,1,1);
	AStaticMeshActor* actor;

	// Parent container
	actor = modelBase->CreateActor(name, location, rotation, scale, spawnParams);
	USceneComponent* parent = actor->FindComponentByClass<USceneComponent>();

	LoadContent* loadContent = LoadContent::GetInstance();
	FString meshCube = loadContent->Mesh("cube");
	FString materialWood = loadContent->Material("wood");

	// // Seat
	// location = FVector(0, 0, size.Z);
	// scale = FVector(size.X, size.Y, 0.2);
	// modelBase->CreateActor(name + "_Seat", location, rotation, scale, spawnParams, parent, meshCube, materialWood);

}
