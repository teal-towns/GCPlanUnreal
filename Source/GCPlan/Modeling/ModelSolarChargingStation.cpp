#include "ModelSolarChargingStation.h"

#include "Engine/StaticMeshActor.h"

#include "ModelBase.h"
#include "../Common/Lodash.h"
#include "../Mesh/LoadContent.h"
#include "../ModelingStructsActor.h"
#include "../ProceduralModel/PMPrism.h"
#include "../Common/Lodash.h"

ModelSolarChargingStation::ModelSolarChargingStation()
{
}

ModelSolarChargingStation::~ModelSolarChargingStation()
{
}

AActor *ModelSolarChargingStation::Create()
{
	ModelBase *modelBase = ModelBase::GetInstance();
	auto [modelingBase, modelParams] = modelBase->GetInputs("SolarChargingStation", FVector(20, 5, 3)); // common size for 8 car parking spaces
	FString name = modelingBase.name;
	FVector size = modelingBase.size;

	FVector rotation = FVector(0, 0, 0);
	FActorSpawnParameters spawnParams;
	FVector location = FVector(0, 0, 0);
	FVector scale = FVector(1, 1, 1);
	AStaticMeshActor *actor;

	// Parent container
	actor = modelBase->CreateActor(name, location, rotation, scale, spawnParams);
	USceneComponent *parent = actor->FindComponentByClass<USceneComponent>();

	LoadContent *loadContent = LoadContent::GetInstance();
	FString materialPath = loadContent->Material("grey");
	FString materialSolarPanelPath = loadContent->Material("solarPanel");

	FString meshPath = loadContent->Mesh("cube");
	FString meshPathCylinder = loadContent->Mesh("cylinder");

	modelParams.parent = parent;
	modelParams.meshPath = meshPath;
	modelParams.materialPath = materialPath;

	float solarPanelWidth = size.X;
	int baseSupportCount = 2 + (size.X / 3 - 1); // Set bases stand 3 meters apart

	CreateBaseSupport(name, scale, "right", size.X, size.Z, spawnParams, modelParams, modelBase);
	CreateBaseSupport(name, scale, "middle", size.X, size.Z, spawnParams, modelParams, modelBase);
	CreateBaseSupport(name, scale, "left", size.X, size.Z, spawnParams, modelParams, modelBase);

	/* Solar Panel */
	modelParams.meshPath = meshPath;
	modelParams.materialPath = materialSolarPanelPath;
	scale = FVector(solarPanelWidth, size.Y, 0.01);
	location = FVector(0, 0, size.Z);
	modelBase->CreateActor(name + "_solarPanel", location, FVector(10, 0, 0), scale, spawnParams, modelParams);

	return actor;
}

// Create base support
void ModelSolarChargingStation::CreateBaseSupport(FString name, FVector scale, FString side, float sizeX, float sizeZ, FActorSpawnParameters spawnParams, FModelParams modelParams, ModelBase *modelBase)
{
	float sideFactor = side == "left" ? -1 : 1;
	name = name + "_" + side;
	float baseWidth = 0.8;
	float baseHeight = sizeZ * 0.2;
	float baseBottomHeight = baseHeight * 0.2;
	float baseBottomOffset = 0.4;
	float baseMiddleHeight = baseHeight * 0.8;
	float baseMiddleWidth = baseWidth * (1 - baseBottomOffset / 2);
	float currentZ = 0;
	float currentX = 0;
	if (side == "right")
	{
		currentX = (sizeX / 2 - baseWidth / 2);
	}
	else if (side == "left")
	{
		currentX = -(sizeX / 2 - baseWidth / 2);
	}

	LoadContent *loadContent = LoadContent::GetInstance();
	FString meshPath = loadContent->Mesh("cube");
	FString meshPathCylinder = loadContent->Mesh("cylinder");
	FString materialSmoothConcretePath = loadContent->Material("smoothConcrete");
	FString materialMetalChromePath = loadContent->Material("metalChrome");
	UStaticMesh *meshTrapezoidalPrism = PMPrism::Create(name + "_TrapezoidalPrism", 4, 1, 1, -0.4, true, true, true);

	/* Base support */
	// Bottom
	FVector location = FVector(currentX, 0, 0);
	scale = FVector(baseWidth, baseWidth, baseBottomHeight);
	modelParams.mesh = meshTrapezoidalPrism;
	modelParams.materialPath = materialSmoothConcretePath;
	modelBase->CreateActor(name + "_BaseBottom", location, FVector(0, 0, 45), scale, spawnParams, modelParams);
	modelParams.mesh = nullptr;

	// Middle Column
	currentZ += baseBottomHeight;
	modelParams.meshPath = meshPath;
	location = FVector(currentX, 0, currentZ);
	scale = FVector(baseMiddleWidth, baseMiddleWidth, baseMiddleHeight);
	modelBase->CreateActor(name + "_BaseMiddle", location, FVector(0, 0, 0), scale, spawnParams, modelParams);

	// Middle Top triangle
	currentZ += baseMiddleHeight;
	float sideLength = baseMiddleWidth * FMath::Sin(FMath::DegreesToRadians(45));
	scale = FVector(sideLength, sideLength, baseMiddleWidth);
	location = FVector(currentX + baseMiddleWidth / 2, 0, currentZ);
	modelBase->CreateActor(name + "_BaseMiddleTopTriangle", location, FVector(90, 45, 90), scale, spawnParams, modelParams);

	// Middle Top Block
	scale = FVector(baseMiddleWidth, baseMiddleWidth * 0.5, baseMiddleWidth * 0.5);
	location = FVector(currentX, -baseMiddleWidth * 0.25, currentZ);
	modelBase->CreateActor(name + "_BaseMiddleTopBlock", location, FVector(0, 0, 0), scale, spawnParams, modelParams);

	// Support Beams
	scale = FVector(baseMiddleWidth * 0.3, baseMiddleWidth * 0.3, sizeZ - currentZ);
	location = FVector(currentX, -baseMiddleWidth * 0.25, currentZ);
	modelParams.meshPath = meshPathCylinder;
	modelParams.materialPath = materialMetalChromePath;
	modelBase->CreateActor(name + "_BaseSupportBeam1", location, FVector(0, 0, 0), scale, spawnParams, modelParams);

	scale = FVector(baseMiddleWidth * 0.3, baseMiddleWidth * 0.3, sizeZ - currentZ);
	location = FVector(currentX, 0, currentZ);
	modelBase->CreateActor(name + "_BaseSupportBeam2", location, FVector(20, 0, 0), scale, spawnParams, modelParams);
}