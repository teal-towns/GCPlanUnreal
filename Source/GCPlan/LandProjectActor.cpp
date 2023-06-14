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
#include "Draw/DrawVertices.h"
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

		VerticesEdit* verticesEdit = VerticesEdit::GetInstance();
		verticesEdit->LoadFromFiles();
		verticesEdit->CheckSubdividePolygons("plot");
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
		VerticesEdit* verticesEdit = VerticesEdit::GetInstance();
		verticesEdit->LoadFromFiles();
		verticesEdit->RemoveChildren("plot");
		UE_LOG(LogTemp, Display, TEXT("Plots children cleared"));
	}
}

void ALandProjectActor::EditorClear() {
	UnrealGlobal* unrealGlobal = UnrealGlobal::GetInstance();
	unrealGlobal->InitAll(GetWorld());

	SplineRoad* splineRoad = SplineRoad::GetInstance();
	splineRoad->CleanUp();

	// Must be last (after any others that call instanced mesh).
	InstancedMesh* instancedMesh = InstancedMesh::GetInstance();
	instancedMesh->CleanUp();
}

void ALandProjectActor::EditorGenerate() {
	UnrealGlobal* unrealGlobal = UnrealGlobal::GetInstance();
	unrealGlobal->InitAll(GetWorld());

	// this->InitSocketOn();
	// this->Login();

	DrawVertices::LoadVertices();
}
