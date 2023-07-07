#include "LandProjectActor.h"

#include "Blueprint/UserWidget.h"
#include "JsonObjectConverter.h"
#include "Engine/World.h"
#include "LevelSequence.h"
#include "LevelSequenceActor.h"
#include "Kismet/GameplayStatics.h"

#include "BuildingStructsActor.h"
#include "DataStructsActor.h"
#include "SocketActor.h"

#include "Common/Lodash.h"
#include "Common/UnrealGlobal.h"
#include "Draw/DrawVertices.h"
#include "Landscape/SplineRoad.h"
#include "Landscape/VerticesEdit.h"
#include "Mesh/InstancedMesh.h"
#include "Plot/PlotBuild.h"

#include "CanvasTextWidget.h"
#include "Draw/DrawHighlight.h"
#include "LayoutModel/TrainStation/LMTrain.h"
#include "Modeling/Common/ModelHighlight.h"
#include "Move/MoveObject.h"
#include "XLisbon/LisbonSequence.h"
#include "XLisbon/LisbonExteriors.h"
#include "XLisbon/LisbonInteriors.h"
#include "ProceduralModel/PMWall.h"
#include "Common/DataConvert.h"
#include "Building/BuildingRoom.h"

#include "Common/MathVector.h"
#include "Layout/LayoutPolygon.h"
#include "LayoutModel/OfficeRoom/LMConferenceRoom.h"

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
	UnrealGlobal* unrealGlobal = UnrealGlobal::GetInstance();
	// TArray<FString> Keys = {"socket"};
	// unrealGlobal->InitAll(GetWorld());
	// if (unrealGlobal->IsIniteds(Keys) && unrealGlobal->SocketActor->IsConnected()) {
	// 	Inited = true;
	// 	// this->InitSocketOn();
	// 	// this->Login();

	// 	VerticesEdit* verticesEdit = VerticesEdit::GetInstance();
	// 	verticesEdit->LoadFromFiles();
	// 	verticesEdit->CheckSubdividePolygons("plot");
	// 	verticesEdit->Hide();

	// 	// CreateUI();

	// 	// MoveObject* moveObject = MoveObject::GetInstance();
	// 	// moveObject->CleanUp();
	// 	// LisbonSequence* lisbonSequence = LisbonSequence::GetInstance();
	// 	// lisbonSequence->Start();

	// 	if (LisbonSequence && LisbonSequence->SequencePlayer) {
	// 		LisbonSequence->SequencePlayer->Play();
	// 	} else {
	// 		UE_LOG(LogTemp, Warning, TEXT("Missing LisbonSequence, skipping"));
	// 	}
	// }

	// if (LisbonSequence && LisbonSequence->SequencePlayer) {
	// 	LisbonSequence->SequencePlayer->Play();
	// } else {
	// 	UE_LOG(LogTemp, Warning, TEXT("Missing LisbonSequence, skipping"));
	// }
}

void ALandProjectActor::GetProject(FString UName) {
	UnrealGlobal* unrealGlobal = UnrealGlobal::GetInstance();
	TMap<FString, FString> Data = {
		{ "uName", UName },
		{ "getPlotsSupercells", "1" }
	};
	unrealGlobal->SocketActor->Emit("projectGetByUName", Data);
}

void ALandProjectActor::TakeAction() {
	UnrealGlobal* unrealGlobal = UnrealGlobal::GetInstance();
	unrealGlobal->InitAll(GetWorld());

	if (EditorParams.Action == EditorActionsLP::PLOTSREMOVECHILDREN) {
		VerticesEdit* verticesEdit = VerticesEdit::GetInstance();
		verticesEdit->LoadFromFiles();
		verticesEdit->RemoveChildren("plot");
		UE_LOG(LogTemp, Display, TEXT("Plots children cleared"));
	}
}

void ALandProjectActor::Test() {
	UnrealGlobal* unrealGlobal = UnrealGlobal::GetInstance();
	unrealGlobal->InitAll(GetWorld());

	// LayoutPolygon* layoutPolygon = LayoutPolygon::GetInstance();
	// TArray<FVector> vertices = {
	// 	// FVector(0,0,0), FVector(0,0,4), FVector(0,3,4), FVector(0,3,0)
	// 	// FVector(-1,1.5,0), FVector(-1,1.5,4), FVector(1.5,-2,4), FVector(1.5,-2,0)
	// 	FVector(-1,-2,0), FVector(-1,-2,4), FVector(1.5,1.5,4), FVector(1.5,1.5,0)
	// };
	// TArray<FString> meshNames = { "brackenFern", "solidFern", "cinnamonFern" };
	// FPlaceParams placeParams;
	// placeParams.plane = "yz";
	// placeParams.offsetAverage = 0.3;
	// placeParams.rotMinX = 90;
	// placeParams.rotMaxX = 90;
	// placeParams.rotMinZ = 270;
	// placeParams.rotMaxZ = 270;
	// placeParams.rotMinY = 0;
	// placeParams.rotMaxY = 360;
	// LayoutPolygon::PlaceInPolygon(vertices, meshNames, placeParams);


	// FModelParams modelParams;
	// FModelCreateParams createParams;
	// LMOfficeDesks::Desks(FVector(10,12,4), modelParams, createParams);

	// TArray<FVector> vertices = { FVector(1, -1, 0) };
	// FVector rotation = FVector(0,0,180);
	// // rotation = FVector(0,0,30);
	// vertices = MathVector::RotateAround(vertices, rotation, FVector(0,0,0));
	// UE_LOG(LogTemp, Display, TEXT("rotate around %s"), *vertices[0].ToString());

	// FModelCreateParams createParams;
	// createParams.offset = FVector(-400,-121,26);
	// createParams.rotation = FVector(0,0,-45);
	// FHighlightText paramsText;
	// // paramsText.text = "testing1";
	// // ModelHighlight::Create(FVector(5,5,5), FModelParams(), createParams, {}, paramsText);
	// DrawHighlight* drawHighlight = DrawHighlight::GetInstance();
	// // paramsText.text = Lodash::GetInstanceId("testing1");
	// // drawHighlight->CreateOne("key1", FVector(5,5,5), FModelParams(), createParams, {}, paramsText);
	// paramsText.text = "Train Station";
	// drawHighlight->CreateOne("trainStation", FVector(5,5,5), FModelParams(), createParams, {}, paramsText);

	// FVector size = FVector(5,10,1);
	// FWall wallParams;
	// float z = 0;
	// // Door 1 meter from left, 1 meter wide, 3 meters tall.
	// float doorLeftY = size.Y / -2 + 1;
	// float doorRightY = doorLeftY + 1;
	// float doorBottomX = size.X / -2;
	// float doorTopX = size.X / -2 + 3;
	// // Window 2 meters from right, 1 meter from bottom, 0.5 meters wide, 2 meters tall.
	// float windowLeftY = size.Y / 2 - 2;
	// float windowRightY = windowLeftY + 0.5;
	// float windowBottomX = size.X / -2 + 1;
	// float windowTopX = windowBottomX + 2;
	// wallParams.holesVertices = {
	// 	{
	// 		FVector(doorBottomX, doorLeftY, z), FVector(doorTopX, doorLeftY, z),
	// 		FVector(doorTopX, doorRightY, z), FVector(doorBottomX, doorRightY, z)
	// 	},
	// 	{
	// 		FVector(windowBottomX, windowLeftY, z), FVector(windowTopX, windowLeftY, z),
	// 		FVector(windowTopX, windowRightY, z), FVector(windowBottomX, windowRightY, z)
	// 	}
	// };
	// FModelCreateParams createParams;
	// FModelParams modelParams;
	// modelParams.materialKey = "leather";
	// PMWall::Create(size, createParams, modelParams, wallParams);
	// // Create other side
	// createParams.offset += FVector(0,0,-0.01);
	// wallParams.triangleDirection = "clockwise";
	// PMWall::Create(size, createParams, modelParams, wallParams);

	// TMap<FString, FString> pairs = Lodash::PairsStringToObject("meshRule=roomWall&mat=white&scale=4,12,0.0100&door1=1&door1YMin=10.50&window1=1&window1YMin=2");
	// // TMap<FString, FString> pairs = Lodash::PairsStringToObject("meshRule=roomWall&mat=white&scale=4,12,0.0100&door1=1&window1=1");
	// FVector location = FVector(0,0,4);
	// pairs.Add("loc", DataConvert::VectorToString(location));
	// BuildingRoom::Build(pairs);


	// FModelParams modelParams;
	// FModelCreateParams createParams;
	// LMConferenceRoom::Create(FVector(10,12,4), modelParams, createParams);

	TArray<FVector> vertices;
	TArray<FString> meshNames = { "brackenFern" };
	FPlaceParams placeParams;
	placeParams.offsetAverage = 0.3;

	placeParams.rotMinY = 265;
	placeParams.rotMaxY = 275;
	placeParams.plane = "yz";
	vertices = {
		FVector(-4.9750,-5.0000,0.5000),
		FVector(-4.9750,-5.0000,3.5000),
		FVector(-4.9750,5.0000,3.5000),
		FVector(-4.9750,5.0000,0.5000)
	};

	// placeParams.rotMinX = 85;
	// placeParams.rotMaxX = 95;
	// placeParams.plane = "xz";
	// vertices = {
	// 	FVector(-4.0000,-5.9750,0.5000),
	// 	FVector(-4.0000,-5.9750,3.5000),
	// 	FVector(4.0000,-5.9750,3.5000),
	// 	FVector(4.0000,-5.9750,0.5000)
	// };

	LayoutPolygon::PlaceInPolygon(vertices, meshNames, placeParams);
}

void ALandProjectActor::Clear() {
	UnrealGlobal* unrealGlobal = UnrealGlobal::GetInstance();
	unrealGlobal->InitAll(GetWorld());

	SplineRoad* splineRoad = SplineRoad::GetInstance();
	splineRoad->CleanUp();

	// Must be last (after any others that call instanced mesh).
	InstancedMesh* instancedMesh = InstancedMesh::GetInstance();
	instancedMesh->CleanUp();

	// Not fixing the issue..
	// GEngine->ForceGarbageCollection(true);
}

void ALandProjectActor::DrawVertices() {
	UnrealGlobal* unrealGlobal = UnrealGlobal::GetInstance();
	unrealGlobal->InitAll(GetWorld());

	// this->InitSocketOn();
	// this->Login();

	DrawVertices::LoadVertices();
}

void ALandProjectActor::SetVertices() {
	UnrealGlobal* unrealGlobal = UnrealGlobal::GetInstance();
	unrealGlobal->InitAll(GetWorld());

	VerticesEdit* verticesEdit = VerticesEdit::GetInstance();
	verticesEdit->DestroyItems();

	// float zOffset = 4;
	float zOffset = 0;
	// TESTING
	// LisbonInteriors::Rooms(zOffset);
	LisbonExteriors::Buildings(zOffset);
	LisbonExteriors::ParkingLots(zOffset);
	LisbonExteriors::Trees(zOffset);

	FModelParams modelParams;
	FModelCreateParams createParams;
	createParams.rotation = FVector(0,0,-60);
	// createParams.offset = FVector(-401,-149,3);
	// createParams.offset = FVector(135,-1124,3);
	createParams.offset = FVector(-65.5,-777,zOffset);
	LMTrain::Create(FVector(0,0,0), modelParams, createParams);
}

void ALandProjectActor::CreateUI() {
	if (!CanvasTextWidget) {
		UE_LOG(LogTemp, Warning, TEXT("LandProjectActor.CreateUI missing CanvasTextWidget"));
	} else {
		CanvasTextWidget->AddToViewport(0);
		// CanvasTextWidget->SetText(Lodash::GetInstanceId("TextHere_"));
		CanvasTextWidget->SetText(Lodash::GetInstanceId(""));
		UnrealGlobal* unrealGlobal = UnrealGlobal::GetInstance();
		unrealGlobal->SetWidgets(CanvasTextWidget);
	}
}

// void ALandProjectActor::InitSequencer() {
// 	UnrealGlobal* unrealGlobal = UnrealGlobal::GetInstance();
// 	unrealGlobal->InitAll(GetWorld());

// 	// CreateUI();
// }
