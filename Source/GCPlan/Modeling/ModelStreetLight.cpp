#include "ModelStreetLight.h"

#include "Engine/StaticMeshActor.h"

#include "ModelBase.h"
#include "../ModelingStructsActor.h"
#include "../ProceduralModel/PMCylinder.h"

ModelStreetLight::ModelStreetLight()
{
}

ModelStreetLight::~ModelStreetLight()
{
}

void ModelStreetLight::Create()
{
    ModelBase *modelBase = ModelBase::GetInstance();
    UWorld *World = modelBase->GetWorld();
    FModelingBase modelingBase = modelBase->GetInputs("StreetLight1", FVector(1, 1, 5));
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

    FString materialPath = "/Script/Engine.Material'/Game/Nature/Wood/wood-pale-material.wood-pale-material'";

    float baseBottomHeight = size.Z * 2 / 5;
    float baseMiddleHeight = size.Z / 5;
    float baseTopHeight = size.Z / 5;
    float yShapeWidth = 0.2;
    float vShapeAngle = 15;
    float vShapeRadian = vShapeAngle * PI / 180;
    float vShapeLength = baseTopHeight / (cos(vShapeRadian));

    float lightBulbRadius = baseTopHeight / 2 - yShapeWidth;
    float lightBulbCenterToBaseTopHeight = lightBulbRadius / sin(vShapeRadian) + lightBulbRadius;

    float solarPanelHeight = size.Z / 5;
    float solarPanelWidth = (tan(vShapeRadian) * baseTopHeight) * 2 + yShapeWidth;

    float currentHeight = 0;

    // Base Bottom
    location = FVector(0, 0, baseBottomHeight / 2);
    scale = FVector(0.3, 0.3, baseBottomHeight);
    modelBase->CreateActor(name + "_BaseBottom", location, rotation, scale, spawnParams, parent, meshPathCylinder, materialPath);
    currentHeight += baseBottomHeight;

    // Base middle
    location = FVector(0, 0, currentHeight + baseMiddleHeight);
    scale = FVector(0.3, 0.3, baseMiddleHeight);
    modelBase->CreateActor(name + "_BaseMiddle", location, rotation, scale, spawnParams, parent, meshPathCylinder, materialPath);
    currentHeight += baseMiddleHeight;

    // Base top V right
    location = FVector(0, 0, currentHeight + baseTopHeight);
    scale = FVector(0.2, 0.2, vShapeLength );
    modelBase->CreateActor(name + "_BaseTopRight", location, FRotator(vShapeAngle, 0, 0), scale, spawnParams, parent, meshPathCylinder, materialPath);

    // Base top V left
    location = FVector(0, 0, currentHeight + baseTopHeight);
    scale = FVector(yShapeWidth, yShapeWidth, vShapeLength );
    modelBase->CreateActor(name + "_BaseTopLeft", location, FRotator(-vShapeAngle, 0, 0), scale, spawnParams, parent, meshPathCylinder, materialPath);


    // Light bulb
    location = FVector(0, 0, currentHeight + lightBulbCenterToBaseTopHeight);
    scale = FVector(lightBulbRadius, lightBulbRadius, lightBulbRadius);
    modelBase->CreateActor(name + "_LightBulb", location, rotation, scale, spawnParams, parent, meshPathSphere, materialPath);

    currentHeight += baseTopHeight;

    // Solar panel
    location = FVector(0, 0, currentHeight + solarPanelHeight);
    scale = FVector(solarPanelWidth, 0.1, solarPanelHeight * 1.5);
    modelBase->CreateActor(name + "_SolarPanel", location, FRotator(0, 0, 30), scale, spawnParams, parent, meshPathCube, materialPath);
}
