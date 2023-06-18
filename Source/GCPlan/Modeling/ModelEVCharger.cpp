#include "ModelEVCharger.h"

#include "Engine/StaticMeshActor.h"

#include "ModelBase.h"
#include "../ModelingStructsActor.h"
#include "../Mesh/LoadContent.h"
#include "../ProceduralModel/PMPrism.h"

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
    auto [modelingBase, modelParams] = modelBase->GetInputs("EVCharger", FVector(1, 1, 2));
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
	modelParams.parent = parent;

    float chargerBottomHeight = size.Z * 2 / 3;
    float chargerTopHeight = size.Z / 3;
    float chargerWidth = 0.8;
    float chargerDepth = 0.3;
    float rectVerticalWidth = chargerWidth * 0.1;
    float rectHorizontalHeight = chargerTopHeight * 0.1;
    float cableLength = 2;
    float cableDiameter = 0.04;

    modelParams.meshPath = meshPathCube;
    // Charger Bottom
    location = FVector(0, 0, 0);
    scale = FVector(chargerWidth, chargerDepth, chargerBottomHeight);
    modelBase->CreateActor(name + "_ChargerBottom", location, rotation, scale, spawnParams, modelParams);

    // Charger Top Rectangular Frame Left
    location = FVector(-(chargerWidth - rectVerticalWidth) / 2, 0, chargerBottomHeight);
    scale = FVector(rectVerticalWidth, chargerDepth, chargerTopHeight);
    modelBase->CreateActor(name + "_ChargerTopRectLeft", location, rotation, scale, spawnParams, modelParams);

    // Charger Top Rectangular Frame Right
    location = FVector((chargerWidth - rectVerticalWidth) / 2, 0, chargerBottomHeight);
    scale = FVector(rectVerticalWidth, chargerDepth, chargerTopHeight);
    modelBase->CreateActor(name + "_ChargerTopRectRight", location, rotation, scale, spawnParams, modelParams);

    // Charger Top Rectangular Frame Top
    location = FVector(0, 0, chargerBottomHeight + chargerTopHeight - rectHorizontalHeight);
    scale = FVector(chargerWidth, chargerDepth, rectHorizontalHeight);
    modelBase->CreateActor(name + "_ChargerTopRectTop", location, rotation, scale, spawnParams, modelParams);

    // Screen
    location = FVector(0, chargerDepth / 2, chargerBottomHeight * 2 / 3);
    scale = FVector(chargerWidth * 0.8, 0.02, chargerBottomHeight * 0.3);
    modelBase->CreateActor(name + "_Screen", location, rotation, scale, spawnParams, modelParams);

    /******************** Charging Cable ********************/

    modelParams.meshPath = meshPathCylinder;
    // Charging Cable Section 1
    float section1Length = cableLength * 0.05;
    float cableCurrentX = (chargerWidth + section1Length) / 2;
    float cableCurrentZ = chargerBottomHeight * 0.8;
    location = FVector(cableCurrentX, 0, cableCurrentZ);
    scale = FVector(cableDiameter, cableDiameter, section1Length);
    modelBase->CreateActor(name + "_ChargingCable_Section1", location, FVector(0, 90, 0), scale, spawnParams, modelParams);

    // Charging Cable Section 2
    float section2Length = cableLength * 0.3;
    cableCurrentZ = cableCurrentZ - section2Length + cableDiameter / 2;
    location = FVector(cableCurrentX, 0, cableCurrentZ);
    scale = FVector(cableDiameter, cableDiameter, section2Length);
    modelBase->CreateActor(name + "_ChargingCable_Section2", location, FVector(0, 0, 0), scale, spawnParams, modelParams);

    // Charging Cable Section 3
    float section3Length = cableLength * 0.2;
    float stepAngleDegree = 15.0;
    float circularAngleDegree = 180.0;
    FVector startLocation = FVector(cableCurrentX, 0, cableCurrentZ);
    float unitLength = (section3Length * stepAngleDegree / circularAngleDegree);

    auto [locations, rotations] = BuildCircularCableLocations(section3Length, 180.0, FVector(cableCurrentX, 0, cableCurrentZ), 15.0, unitLength);
    for (int32 i = 0; i < locations.Num() - 1; i++)
    {
        location = locations[i];
        rotation = rotations[i];
        scale = FVector(cableDiameter, cableDiameter, unitLength * 1.1); // unitLength * 1.1 to avoid gap between units due to rotation
        modelBase->CreateActor(name + "__ChargingCable_Section3_" + FString::FromInt(i), location, rotation, scale, spawnParams, modelParams);
    }

    // Charging Cable Section 4
    float section4Length = cableLength * 0.15;
    cableCurrentX = cableCurrentX + section3Length * 2 / PI;
    location = FVector(cableCurrentX, 0, cableCurrentZ);
    scale = FVector(cableDiameter, cableDiameter, section4Length);
    modelBase->CreateActor(name + "_ChargingCable_Section4", location, FVector(0, 15, 0), scale, spawnParams, modelParams);

    // Charging Cable Section 5
    float section5Length = cableLength * 0.3;
    cableCurrentZ = cableCurrentZ + section4Length * FMath::Cos(FMath::DegreesToRadians(15.0));
    cableCurrentX = cableCurrentX - section4Length * FMath::Sin(FMath::DegreesToRadians(15.0));
    location = FVector(cableCurrentX, 0, cableCurrentZ);
    scale = FVector(cableDiameter, cableDiameter, section5Length);
    modelBase->CreateActor(name + "_ChargingCable_Section5", location, FVector(0, 0, 0), scale, spawnParams, modelParams);

    /*************** Connector ***************/
    float connectorLength = cableCurrentX - chargerWidth / 2;

    // Connector Bottom
    float connectorBottomLength = connectorLength * 0.2;
    float connectorBottomDiameter = cableDiameter * 1.5;
    float connectorBottomAngleDegree = 30.0;
    cableCurrentZ += section5Length * 0.98; // 0.98 so that there's no gap between the cable and the connector
    location = FVector(cableCurrentX, 0, cableCurrentZ);
    scale = FVector(connectorBottomDiameter, connectorBottomDiameter, connectorBottomLength);
    modelBase->CreateActor(name + "_ConnectorBottom", location, FVector(0, connectorBottomAngleDegree, 0), scale, spawnParams, modelParams);

    // Connector Body Section 1
    float connectorBodyS1Width = connectorBottomDiameter * FMath::Cos(FMath::DegreesToRadians(60));
    cableCurrentX = cableCurrentX - connectorBottomDiameter * FMath::Sin(FMath::DegreesToRadians(connectorBottomAngleDegree));
    cableCurrentZ += connectorBottomLength / FMath::Cos(FMath::DegreesToRadians(connectorBottomAngleDegree));
    UStaticMesh *mesh = PMPrism::Create(name + "_ConnectorBodyPrism", {}, 3, 1, 1, 0, true, true, true);
    location = FVector(cableCurrentX, -connectorBottomDiameter * 0.5, cableCurrentZ);
    scale = FVector(connectorBodyS1Width, connectorBodyS1Width, connectorBottomDiameter);
    modelParams.mesh = mesh;
    modelBase->CreateActor(name + "_ConnectorBody_Section1", location, FVector(90, 0, 0), scale, spawnParams, modelParams);
    modelParams.mesh = nullptr;

    modelParams.meshPath = meshPathCube;
    // Connector Body Section 2
    float connectorBodyS2Length = connectorBodyS1Width * 4;
    float connectorBodyS2Width = connectorBodyS2Length / 2;
    cableCurrentX = cableCurrentX - connectorBodyS2Length / 2 - connectorBodyS1Width / 2;
    cableCurrentZ = cableCurrentZ - connectorBodyS2Width * 0.5;
    location = FVector(cableCurrentX, 0, cableCurrentZ);
    scale = FVector(connectorBodyS2Length, connectorBottomDiameter, connectorBodyS2Width);
    modelBase->CreateActor(name + "_ConnectorBody_Section2", location, FVector(0, 0, 0), scale, spawnParams, modelParams);

    modelParams.meshPath = meshPathCylinder;
    // Connector Head Pin 1
    float connectorHeadDiameter = connectorBottomDiameter / 3;
    float connectorHeadPin1Length = ((cableCurrentX - chargerWidth / 2) - connectorBodyS2Length / 2) * 0.9;
    cableCurrentX = chargerWidth / 2 + connectorHeadPin1Length * 1.1;
    cableCurrentZ = cableCurrentZ + connectorBodyS2Width - connectorHeadDiameter;
    location = FVector(cableCurrentX, 0, cableCurrentZ);
    scale = FVector(connectorHeadDiameter, connectorHeadDiameter, connectorHeadPin1Length);
    modelBase->CreateActor(name + "_ConnectorHeadPin1", location, FVector(0, 90, 0), scale, spawnParams, modelParams);

    // Connector Head Pin 2
    float connectorHeadPin2Length = connectorHeadPin1Length * 0.5;
    cableCurrentZ = cableCurrentZ - connectorHeadDiameter * 1.4;
    location = FVector(cableCurrentX, 0, cableCurrentZ);
    scale = FVector(connectorHeadDiameter, connectorHeadDiameter, connectorHeadPin2Length);
    modelBase->CreateActor(name + "_ConnectorHeadPin2", location, FVector(0, 90, 0), scale, spawnParams, modelParams);
}

std::tuple<TArray<FVector>, TArray<FVector>> ModelEVCharger::BuildCircularCableLocations(float totalLength,
	float circularAngleDegree, FVector startLocation, float stepAngleDegree, float unitLength) {
    TArray<FVector> locations;
    TArray<FVector> rotations;

    for (float angleDegree = stepAngleDegree; angleDegree <= circularAngleDegree; angleDegree += stepAngleDegree)
    {
        float X = unitLength * FMath::Sin(FMath::DegreesToRadians(angleDegree));
        float Z = unitLength * FMath::Cos(FMath::DegreesToRadians(angleDegree));
        FVector location = FVector(X + startLocation.X, startLocation.Y, startLocation.Z - Z);
        startLocation = location;
        FVector rotation = FVector(0, angleDegree, 0);
        locations.Add(location);
        rotations.Add(rotation);
    }

    return {locations, rotations};
}