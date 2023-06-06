#include "LandProjectActor.h"
#include "JsonObjectConverter.h"
#include "Engine/World.h"
#include "GCPlanGameInstance.h"
#include "Plots/PlotFillVoronoi.h"
#include "Kismet/GameplayStatics.h"

#include "BuildingStructsActor.h"
#include "SettingsActor.h"
#include "SocketActor.h"

#include "LandscapeComponent.h"
// #include "ProceduralMeshComponent.h"

#include "Building/BuildingRing.h"
#include "Building/BuildingRoad.h"
#include "Building/BuildingFlowerHomes.h"
#include "Common/MathPolygon.h"
#include "Common/UnrealGlobal.h"
#include "Landscape/HeightMap.h"
#include "Landscape/MeshTerrain.h"
#include "Landscape/SplineRoad.h"
#include "Mesh/InstancedMesh.h"
#include "Modeling/ModelBase.h"
#include "ProceduralModel/PMBase.h"

ALandProjectActor::ALandProjectActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

void ALandProjectActor::BeginPlay()
{
	Super::BeginPlay();
}

void ALandProjectActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!Inited) {
		Init();
	}
}

void ALandProjectActor::InitSocketOn() {
	SocketActor->On("login", [this](FString DataString) {
		FDataLogin* Data = new FDataLogin();
		if (!FJsonObjectConverter::JsonObjectStringToUStruct(DataString, Data, 0, 0)) {
			UE_LOG(LogTemp, Error, TEXT("LandProjectActor.On login json parse error"));
			// GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "LandProjectActor.On login json parse error");
		} else {
			if (Data->valid > 0) {
				FString ProjectUName = SettingsActor->ProjectUName;
				if (ProjectUName.Len() > 0) {
					this->GetProject(ProjectUName);
				}
			}
		}
	});

	SocketActor->On("projectGetByUName", [this](FString DataString) {
		FDataProject* Data = new FDataProject();
		if (!FJsonObjectConverter::JsonObjectStringToUStruct(DataString, Data, 0, 0)) {
			UE_LOG(LogTemp, Error, TEXT("LandProjectActor.On projectGetByUName json parse error"));
			// GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "LandProjectActor.On projectGetByUName json parse error");
		} else {
			if (Data->valid > 0) {
				this->GenerateLands(Data->lands);
			}
		}
	});
}

void ALandProjectActor::Login() {
	FString LoginEmail = SettingsActor->LoginEmail;
	FString LoginPassword = SettingsActor->LoginPassword;
	FString ProjectUName = SettingsActor->ProjectUName;
	if (LoginEmail.Len() > 0 && LoginPassword.Len() > 0 && ProjectUName.Len() > 0) {
		TMap<FString, FString> Data = {
			{ "email", LoginEmail },
			{ "password", LoginPassword }
		};
		SocketActor->Emit("login", Data);
	}
}

void ALandProjectActor::Init() {
	UGCPlanGameInstance* GameInstance = Cast<UGCPlanGameInstance>(GetGameInstance());
	TArray<FString> Keys = {"settings", "socket"};
	if (GameInstance && GameInstance->IsIniteds(Keys) && GameInstance->SocketActor->IsConnected()) {
		Inited = true;
		// ASettingsActor* SettingsActor = GameInstance->SettingsActor;
		SettingsActor = GameInstance->SettingsActor;

		SocketActor = GameInstance->SocketActor;

		this->InitSocketOn();
		this->Login();
	}
}

void ALandProjectActor::GetProject(FString UName) {
	TMap<FString, FString> Data = {
		{ "uName", UName },
		{ "getPlotsSupercells", "1" }
	};
	SocketActor->Emit("projectGetByUName", Data);
}

void ALandProjectActor::GenerateLands(TArray<FLand> lands) {
	UnrealGlobal* unrealGlobal = UnrealGlobal::GetInstance();
	InstancedMesh* instancedMesh = InstancedMesh::GetInstance();
	// TODO - Unity hex prefab was rotated; reset to 0 rotation as default then remove this.
	bool YIsUp = false;
	float rotationYOffset = 0.0;
	if (YIsUp) {
		rotationYOffset = 30.0;
	}
	// Move up to be above land.
	float zOffset = 0;
	for (int ii = 0; ii < lands.Num(); ii++) {
		UE_LOG(LogTemp, Display, TEXT("land_id %s num GO %d"), *lands[ii].land_id, lands[ii].game_objects.Num());
		for (auto& Elem : lands[ii].game_objects) {
			FLandGameObject GO = Elem.Value;
			FVector Translation = FVector(GO.position["x"] * unrealGlobal->Scale(), GO.position["y"] * unrealGlobal->Scale(), GO.position["z"] * unrealGlobal->Scale() + zOffset);
			if (YIsUp) {
				// Stored in Unity style so swap z and y and convert from meters to centimeters.
				Translation = FVector(GO.position["x"] * unrealGlobal->Scale(), GO.position["z"] * unrealGlobal->Scale(), GO.position["y"] * unrealGlobal->Scale() + zOffset);
			}
			FRotator Rotation = FRotator(GO.rotation["x"], GO.rotation["y"] + rotationYOffset, GO.rotation["z"]);
			FVector Scale = FVector(GO.scale["x"], GO.scale["y"], GO.scale["z"]);
			// UE_LOG(LogTemp, Display, TEXT("GO %f %s %s"), GO.position["z"], *Translation.ToString(), *Rotation.ToString());
			instancedMesh->CreateInstance("HexModule", Translation, Rotation, Scale);
		}
	}
}

void ALandProjectActor::EditorClear() {
	UnrealGlobal* unrealGlobal = UnrealGlobal::GetInstance();
	unrealGlobal->InitAll(GetWorld());

	InstancedMesh* instancedMesh = InstancedMesh::GetInstance();
	instancedMesh->ClearInstances("HexModule");
	SplineRoad* splineRoad = SplineRoad::GetInstance();
	splineRoad->DestroyRoads();
}

void ALandProjectActor::EditorGenerate() {
	UnrealGlobal* unrealGlobal = UnrealGlobal::GetInstance();
	unrealGlobal->InitAll(GetWorld());

	ModelBase* modelBase = ModelBase::GetInstance();
	PMBase* pmBase = PMBase::GetInstance();
	HeightMap* heightMap = HeightMap::GetInstance();
	MeshTerrain* meshTerrain = MeshTerrain::GetInstance();
	SplineRoad* splineRoad = SplineRoad::GetInstance();
	UWorld* World = GetWorld();

	// TODO - this is probably broken now; will need to fix to re-use.
	// this->InitSocketOn();
	// this->Login();

 //	// https://forums.unrealengine.com/t/making-heightmaps-with-c/270928
 //	// https://forums.unrealengine.com/t/access-to-landscape-in-c/397363/2
 //	ALandscape* Landscape;
	// UGameplayStatics::GetAllActorsOfClass(world, ALandscape::StaticClass(), OutActors);
 //	for (AActor* a : OutActors) {
 //		Landscape = Cast<ALandscape>(a);
 //		UE_LOG(LogTemp, Display, TEXT("LPA yes Landscape"));
 //		break;
 //	}
 //	heightMap->SetHeightMap(Landscape);

	bool doRoads = true;
	if (true) {
	FVector posCurrentGround = FVector(200,-650,0);

 //	FVector StartLocation{ posCurrentGround.X, posCurrentGround.Y, 9000 * unrealGlobal->Scale() };
	// FVector EndLocation{ posCurrentGround.X, posCurrentGround.Y, -1000 * unrealGlobal->Scale() };
	// FHitResult HitResult;
	// world->LineTraceSingleByObjectType(
	// 	OUT HitResult,
	// 	StartLocation,
	// 	EndLocation,
	// 	FCollisionObjectQueryParams(ECollisionChannel::ECC_WorldStatic),
	// 	FCollisionQueryParams()
	// );
	// UE_LOG(LogTemp, Display, TEXT("GetZFromWorld1 %s"), *posCurrentGround.ToString());

	// FColor LineColor;
	// if (HitResult.GetActor()) LineColor = FColor::Red;
	// else LineColor = FColor::Green;
	// DrawDebugLine(
	// 	world,
	// 	StartLocation,
	// 	EndLocation,
	// 	LineColor,
	// 	true,
	// 	5.f,
	// 	0.f,
	// 	10.f
	// );

	// if (HitResult.GetActor()) {
	// 	float z1 = HitResult.ImpactPoint.Z;
	// 	UE_LOG(LogTemp, Display, TEXT("z1 %f"), z1);
	// }

 //	FString pattern = "ring";
	// pattern = "flowerHomes";
	// float z = 200;
	// float z = HeightMap::GetTerrainHeightAtPoint(posCurrentGround);
	// heightMap->SetWorld(GetWorld());
	// float z = HeightMap::GetInstance()->GetTerrainHeightAtPoint(posCurrentGround);
	float z = heightMap->GetTerrainHeightAtPoint(posCurrentGround);
	// z = 200;
	// posCurrentGround.Z = z;
	TMap<FString, FPlot> Plots = {
		// { "plot1", { "id1", "plot1", FVector(0,0,0), { FVector(269.0, -767, 82), FVector(159, -85, 152), FVector(962, -85, 153), FVector(962, -586, 70) } } },
		// { "plot2", { "id2", "plot2", FVector(0,0,0), { FVector(200.0, -700, 100), FVector(150, -500, 100), FVector(300, -500, 100), FVector(350, -500, 100) } } },
		// { "plot3", { "id3", "plot3", FVector(200,-650,z), { FVector(200.0, -800, z), FVector(50, -600, z), FVector(225, -500, z), FVector(350, -700, z) } } },
		// { "plot4", { "id4", "plot4", FVector(200,-650,z), { FVector(200.0, -1000, z), FVector(0, -600, z), FVector(225, -400, z), FVector(500, -700, z) },
		// 	"flowerHomes", 150 } },
		{ "marinaRings", {"id5", "plot5", FVector(0,0,z),
			{ FVector(-409, -689, z), FVector(-201, -484, z), FVector(-95, -570, z), FVector(10, -554, z), FVector(-56, -384, z), FVector(-200, -200, z), FVector(-520, -580, z) },
			"ring", 80 }
		},
		{ "marinaVillage", {"id6", "plot6", FVector(0,0,z),
			{ FVector(114, -444, z), FVector(-182, -187, z), FVector(-284, 414, z), FVector(524, 421, z) },
			"flowerHomes", 150 }
		},
	};

	// meshTerrain->DestroyRoads();
	splineRoad->DestroyRoads();
	float minRadiusSkip = 15;

	for (auto& Elem : Plots) {
		TMap<FString, FPlot> PlotsTemp = { { Elem.Key, Elem.Value } };
		float plotDistance = Elem.Value.averagePlotDistance;
		FString pattern = Elem.Value.buildPattern;

		auto [spacesVertices, posCenter, boundsRect] = PlotFillVoronoi::Fill(PlotsTemp, plotDistance);
		UE_LOG(LogTemp, Display, TEXT("LPA spacesVertices.Num %d %s %s"), spacesVertices.Num(), *boundsRect[0].ToString(), *boundsRect[1].ToString());
		float verticesBuffer = -25;

		// Buffer vertices and remove any spaces that are not valid.
		if (verticesBuffer != 0) {
			spacesVertices = PlotFillVoronoi::BufferAndRemoveVertices(spacesVertices, verticesBuffer,
				minRadiusSkip);
		}

		TArray<int> heightFloorsOrder = { 2, 10, 4, 7 };
		TArray<FLand> lands = {};
		FBuildingBlueprint blueprint;
		for (int ii = 0; ii < spacesVertices.Num(); ii++) {
			if (pattern == "flowerHomes") {
				auto [blueprint1, homePlotPaths] = BuildingFlowerHomes::Create(spacesVertices[ii], verticesBuffer, 0.15, 2000, "outer");
				blueprint = blueprint1;
				if (doRoads) {
				// meshTerrain->AddRoads(homePlotPaths);
				// TODO? Or leave these out?
				}
			} else {
				blueprint = BuildingRing::Create(spacesVertices[ii], heightFloorsOrder);
			}
			lands.Add(blueprint.land);
		}
		this->GenerateLands(lands);

		if (doRoads) {
		TMap<FString, FRoadPath> roads = BuildingRoad::BetweenSpaces(spacesVertices, verticesBuffer);
		// meshTerrain->AddRoads(roads);
		splineRoad->AddRoads(roads);
		}
	}
	if (doRoads) {
	// meshTerrain->DrawRoads();
	splineRoad->DrawRoads();
	}
	}
}
