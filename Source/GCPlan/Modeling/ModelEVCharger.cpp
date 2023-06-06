#include "ModelEVCharger.h"

#include "Engine/StaticMeshActor.h"

#include "ModelBase.h"
#include "../ModelingStructsActor.h"
#include "../ProceduralModel/PMCylinder.h"

ModelEVCharger::ModelEVCharger()
{
}

ModelEVCharger::~ModelEVCharger()
{
}

void ModelEVCharger::Create()
{
    ModelBase *modelBase = ModelBase::GetInstance();
    UWorld *World = modelBase->GetWorld();
    FModelingBase modelingBase = modelBase->GetInputs("EVCharger", FVector(1, 1, 2));
    FString name = modelingBase.name;
    FVector size = modelingBase.size;
    TArray<FString> tags = modelingBase.tags;

    FRotator rotation = FRotator(0, 0, 0);
    FActorSpawnParameters spawnParams;
    FVector location = FVector(0, 0, 0);
    FVector scale = FVector(1, 1, 1);
    AStaticMeshActor *actor;

    // Parent container
    actor = modelBase->CreateActor(name, location, rotation, scale, spawnParams);
    USceneComponent *parent = actor->FindComponentByClass<USceneComponent>();

    FString meshPathCube = "/Script/Engine.StaticMesh'/Game/Modeling/Primitives/Cube.Cube'";
    FString meshPathCylinder = "/Script/Engine.StaticMesh'/Game/Modeling/Primitives/Cylinder.Cylinder'";
    FString meshPathSphere = "/Script/Engine.StaticMesh'/Game/Modeling/Primitives/Sphere.Sphere'";

    float chargerBottomHeight = size.Z * 2 / 3;
    float chargerTopHeight = size.Z / 3;
    float chargerWidth = 0.5;
    float rectVerticalWidth = chargerWidth * 0.1;
    float rectHorizontalHeight = chargerTopHeight * 0.1;

    // Charger Bottom
    location = FVector(0, 0, chargerBottomHeight / 2);
    scale = FVector(chargerWidth, 0.3, chargerBottomHeight);
    modelBase->CreateActor(name + "_ChargerBottom", location, rotation, scale, spawnParams, parent, meshPathCube);

    // Charger Top Rectangular Frame Left
    location = FVector(-(chargerWidth - rectVerticalWidth) / 2, 0, chargerBottomHeight + chargerTopHeight);
    scale = FVector(rectVerticalWidth, 0.3, chargerTopHeight);
    modelBase->CreateActor(name + "_ChargerTopRectLeft", location, rotation, scale, spawnParams, parent, meshPathCube);

    // Charger Top Rectangular Frame Right
    location = FVector((chargerWidth - rectVerticalWidth) / 2, 0, chargerBottomHeight + chargerTopHeight);
    scale = FVector(rectVerticalWidth, 0.3, chargerTopHeight);
    modelBase->CreateActor(name + "_ChargerTopRectRight", location, rotation, scale, spawnParams, parent, meshPathCube);

    // Charger Top Rectangular Frame Top
    location = FVector(0, 0, chargerBottomHeight + chargerTopHeight * 2 - rectHorizontalHeight);
    scale = FVector(chargerWidth, 0.3, rectHorizontalHeight);
    modelBase->CreateActor(name + "_ChargerTopRectTop", location, rotation, scale, spawnParams, parent, meshPathCube);

    // Charging Cable Slot
}