#include "ModelStreetLight.h"

#include "Engine/StaticMeshActor.h"

#include "ModelBase.h"
#include "../ModelingStructsActor.h"
#include "../Mesh/LoadContent.h"
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
    auto [modelingBase, modelParams] = modelBase->GetInputs("StreetLight1", FVector(1, 1, 5));
    FString name = modelingBase.name;
    FVector size = modelingBase.size;
    TArray<FString> tags = modelingBase.tags;

    FVector rotation = FVector(0,0,0);
    FActorSpawnParameters spawnParams;
    FVector location = FVector(0, 0, 0);
    FVector scale = FVector(1, 1, 1);
    AStaticMeshActor *actor;

    // Parent container
    actor = modelBase->CreateActor(name, location, rotation, scale, spawnParams);
    USceneComponent *parent = actor->FindComponentByClass<USceneComponent>();

    LoadContent* loadContent = LoadContent::GetInstance();
    FString meshPathCube = loadContent->Mesh("cube");
    FString meshPathCylinder = loadContent->Mesh("cylinder");
    FString meshPathSphere = loadContent->Mesh("sphere");

    FString materialPath = loadContent->Material("wood");

	modelParams.materialPath = materialPath;
	modelParams.parent = parent;

    float baseBottomHeight = size.Z * 2 / 5;
    float baseMiddleHeight = size.Z / 5;
    float baseTopHeight = size.Z / 5;
    float vShapeWidth = 0.2;
    float vShapeAngle = 15;
    float vShapeRadian = vShapeAngle * PI / 180;
    float vShapeLength = baseTopHeight / (cos(vShapeRadian));
    float lightBulbHeight = baseTopHeight * 2 / 3;
    float lightBulbRadius = lightBulbHeight * sin(vShapeRadian) / (1 - sin(vShapeRadian));
    float solarPanelHeight = size.Z / 5;
    float solarPanelWidth = (tan(vShapeRadian) * baseTopHeight) * 2 + vShapeWidth;

    float currentHeight = 0;

    modelParams.meshPath = meshPathCylinder;
    // Base Bottom
    location = FVector(0, 0, 0);
    scale = FVector(0.3, 0.3, baseBottomHeight);
    modelBase->CreateActor(name + "_BaseBottom", location, rotation, scale, spawnParams, modelParams);
    currentHeight += baseBottomHeight;

    // Base middle
    location = FVector(0, 0, currentHeight);
    scale = FVector(0.3, 0.3, baseMiddleHeight);
    modelBase->CreateActor(name + "_BaseMiddle", location, rotation, scale, spawnParams, modelParams);
    currentHeight += baseMiddleHeight;

    // Base top V right
    location = FVector(0, 0, currentHeight);
    scale = FVector(vShapeWidth, vShapeWidth, vShapeLength);
    modelBase->CreateActor(name + "_BaseTopRight", location, FVector(0, vShapeAngle, 0), scale, spawnParams, modelParams);

    // Base top V left
    location = FVector(0, 0, currentHeight);
    scale = FVector(vShapeWidth, vShapeWidth, vShapeLength);
    modelBase->CreateActor(name + "_BaseTopLeft", location, FVector(0, -vShapeAngle, 0), scale, spawnParams, modelParams);


    modelParams.meshPath = meshPathSphere;
    // Light bulb
    location = FVector(0, 0, currentHeight + lightBulbHeight);
    scale = FVector(lightBulbRadius, lightBulbRadius, lightBulbRadius);
    modelBase->CreateActor(name + "_LightBulb", location, rotation, scale, spawnParams, modelParams);

    currentHeight += baseTopHeight;

    modelParams.meshPath = meshPathCube;
    // Solar panel
    location = FVector(0, 0, currentHeight);
    scale = FVector(solarPanelWidth, 0.1, solarPanelHeight * 1.5);
    modelBase->CreateActor(name + "_SolarPanel", location, FVector(30, 0, 0), scale, spawnParams, modelParams);
}
