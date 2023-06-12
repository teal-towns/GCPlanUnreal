#include "LandProjectActor.h"
#include "JsonObjectConverter.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "LandscapeComponent.h"

#include "BuildingStructsActor.h"
#include "DataStructsActor.h"
#include "SocketActor.h"

#include "Building/BuildingRing.h"
#include "Building/BuildingRoad.h"
#include "Building/BuildingFlowerHomes.h"
#include "Common/MathPolygon.h"
#include "Common/UnrealGlobal.h"
// #include "Landscape/HeightMap.h"
#include "Landscape/LandNature.h"
#include "Landscape/MeshTerrain.h"
#include "Landscape/SplineRoad.h"
#include "Landscape/VerticesEdit.h"
#include "Layout/LayoutPolygon.h"
#include "Layout/LayoutPolyLine.h"
#include "Mesh/InstancedMesh.h"
#include "Mesh/LoadContent.h"
// #include "Modeling/ModelBase.h"
#include "Plot/PlotBuild.h"
#include "Plot/PlotData.h"
#include "Plot/PlotDivide.h"
// #include "ProceduralModel/PMBase.h"

ALandProjectActor::ALandProjectActor()
{
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
	UnrealGlobal* unrealGlobal = UnrealGlobal::GetInstance();

	unrealGlobal->SocketActor->On("login", [this, unrealGlobal](FString DataString) {
		FDataLogin* Data = new FDataLogin();
		if (!FJsonObjectConverter::JsonObjectStringToUStruct(DataString, Data, 0, 0)) {
			UE_LOG(LogTemp, Error, TEXT("LandProjectActor.On login json parse error"));
			// GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "LandProjectActor.On login json parse error");
		} else {
			if (Data->valid > 0) {
				FString ProjectUName = unrealGlobal->_settings->projectUName;
				if (ProjectUName.Len() > 0) {
					this->GetProject(ProjectUName);
				}
			}
		}
	});

	unrealGlobal->SocketActor->On("projectGetByUName", [this](FString DataString) {
		FDataProject* Data = new FDataProject();
		if (!FJsonObjectConverter::JsonObjectStringToUStruct(DataString, Data, 0, 0)) {
			UE_LOG(LogTemp, Error, TEXT("LandProjectActor.On projectGetByUName json parse error"));
			// GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "LandProjectActor.On projectGetByUName json parse error");
		} else {
			if (Data->valid > 0) {
				PlotBuild::DrawLands(Data->lands);
			}
		}
	});
}

void ALandProjectActor::Login() {
	UnrealGlobal* unrealGlobal = UnrealGlobal::GetInstance();
	FString LoginEmail = unrealGlobal->_settings->loginEmail;
	FString LoginPassword = unrealGlobal->_settings->loginPassword;
	FString ProjectUName = unrealGlobal->_settings->projectUName;
	if (LoginEmail.Len() > 0 && LoginPassword.Len() > 0 && ProjectUName.Len() > 0) {
		TMap<FString, FString> Data = {
			{ "email", LoginEmail },
			{ "password", LoginPassword }
		};
		unrealGlobal->SocketActor->Emit("login", Data);
	}
}

void ALandProjectActor::Init() {
	TArray<FString> Keys = {"socket"};
	UnrealGlobal* unrealGlobal = UnrealGlobal::GetInstance();
	// unrealGlobal->InitAll(GetWorld());
	if (unrealGlobal->IsIniteds(Keys) && unrealGlobal->SocketActor->IsConnected()) {
		Inited = true;
		this->InitSocketOn();
		this->Login();

		PlotData* plotData = PlotData::GetInstance();
		TMap<FString, FPlot> plots = plotData->LoadAndSubdividePlots();

		VerticesEdit* verticesEdit = VerticesEdit::GetInstance();
		verticesEdit->ImportPolygons(ABuildingStructsActor::PlotsToPolygons(plots));
		UE_LOG(LogTemp, Display, TEXT("init %d"), plots.Num());
		verticesEdit->AddOnSavePolygon("LandProjectActor", [this, verticesEdit, unrealGlobal, plotData](FString uName, FPolygon polygon) {
			UE_LOG(LogTemp, Display, TEXT("on save polygon uName %s type %s"), *uName, *polygon.type);
			// TMap<FString, FPolygon> polygons = verticesEdit->ExportPolygonsByType(type);
			if (polygon.type == "plot") {
				plotData->SavePlotFromPolygon(uName, polygon);
			}
		});
		verticesEdit->AddOnDeletePolygon("LandProjectActor", [this, verticesEdit, unrealGlobal, plotData](FString uName, FString type) {
			UE_LOG(LogTemp, Display, TEXT("on delete polygon uName %s type %s"), *uName, *type);
			if (type == "plot") {
				plotData->DeletePlot(uName);
			}
		});
	}
}

void ALandProjectActor::GetProject(FString UName) {
	UnrealGlobal* unrealGlobal = UnrealGlobal::GetInstance();
	TMap<FString, FString> Data = {
		{ "uName", UName },
		{ "getPlotsSupercells", "1" }
	};
	unrealGlobal->SocketActor->Emit("projectGetByUName", Data);
}

void ALandProjectActor::EditorTakeAction() {
	UnrealGlobal* unrealGlobal = UnrealGlobal::GetInstance();
	unrealGlobal->InitAll(GetWorld());

	if (EditorParams.Action == EditorActionsLP::PLOTSREMOVECHILDREN) {
		PlotData* plotData = PlotData::GetInstance();
		plotData->LoadPlots();
		plotData->RemoveChildren();
		UE_LOG(LogTemp, Display, TEXT("Plots children cleared"));
	}
}

void ALandProjectActor::EditorClear() {
	UnrealGlobal* unrealGlobal = UnrealGlobal::GetInstance();
	unrealGlobal->InitAll(GetWorld());

	InstancedMesh* instancedMesh = InstancedMesh::GetInstance();
	instancedMesh->CleanUp();
	SplineRoad* splineRoad = SplineRoad::GetInstance();
	splineRoad->DestroyRoads();
}

void ALandProjectActor::EditorGenerate() {
	UnrealGlobal* unrealGlobal = UnrealGlobal::GetInstance();
	unrealGlobal->InitAll(GetWorld());

	// this->InitSocketOn();
	// this->Login();

	PlotData* plotData = PlotData::GetInstance();
	TMap<FString, FPlot> plots = plotData->LoadAndSubdividePlots();

	// VerticesEdit* verticesEdit = VerticesEdit::GetInstance();
	// verticesEdit->ImportPolygons(ABuildingStructsActor::PlotsToPolygons(plots));

	TArray<FLand> lands = PlotBuild::CreateLands(plots);
	PlotBuild::DrawLands(lands);

	// MeshTerrain* meshTerrain = MeshTerrain::GetInstance();
	SplineRoad* splineRoad = SplineRoad::GetInstance();
	// meshTerrain->DrawRoads();
	splineRoad->DrawRoads();

	// Place nature on land.
	if (unrealGlobal->_settings->performanceQualityLevel >= 8) {
		LandNature::PlaceNature(plots);
	}


	// FVector posCurrentGround = FVector(200,-650,0);
	// float z = heightMap->GetTerrainHeightAtPoint(posCurrentGround);
	// TMap<FString, FPlot> Plots = {
	// 	// { "plot1", { "id1", "plot1", { FVector(269.0, -767, 82), FVector(159, -85, 152), FVector(962, -85, 153), FVector(962, -586, 70) }, FVector(0,0,0) } },
	// 	// { "plot2", { "id2", "plot2", { FVector(200.0, -700, 100), FVector(150, -500, 100), FVector(300, -500, 100), FVector(350, -500, 100) }, FVector(0,0,0) } },
	// 	// { "plot3", { "id3", "plot3", { FVector(200.0, -800, z), FVector(50, -600, z), FVector(225, -500, z), FVector(350, -700, z) }, FVector(200,-650,z) } },
	// 	// { "plot4", { "id4", "plot4", { FVector(200.0, -1000, z), FVector(0, -600, z), FVector(225, -400, z), FVector(500, -700, z) }, FVector(200,-650,z),
	// 	// 	"flowerHomes", 150 } },
	// 	{ "marinaRings", {"id5", "plot5",
	// 		{ FVector(-409, -689, z), FVector(-201, -484, z), FVector(-95, -570, z), FVector(10, -554, z), FVector(-56, -384, z), FVector(-200, -200, z), FVector(-520, -580, z) },
	// 		FVector(0,0,z), "ring", 80 }
	// 	},
	// 	{ "marinaVillage", {"id6", "plot6",
	// 		{ FVector(114, -444, z), FVector(-182, -187, z), FVector(-284, 414, z), FVector(524, 421, z) },
	// 		FVector(0,0,z), "flowerHomes", 150 }
	// 	},
	// };
}
