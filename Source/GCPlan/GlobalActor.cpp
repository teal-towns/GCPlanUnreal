#include "GlobalActor.h"

#include "Fonts/SlateFontInfo.h"

#include "CanvasTextWidget.h"
#include "Common/Lodash.h"
#include "Common/MathVector.h"
#include "Common/UnrealGlobal.h"
#include "Move/MoveObject.h"
#include "Move/MovePolyLine.h"
#include "XLisbon/LisbonSequence.h"
#include "XLisbon/LisbonWorldIntro.h"

AGlobalActor::AGlobalActor()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AGlobalActor::BeginPlay()
{
	Super::BeginPlay();
	// TESTING
	// LisbonWorld();
}

void AGlobalActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	MoveObject* moveObject = MoveObject::GetInstance();
	moveObject->Tick(DeltaTime);
	MovePolyLine* movePolyLine = MovePolyLine::GetInstance();
	movePolyLine->Tick(DeltaTime);
}

// void AGlobalActor::Init() {
// 	UnrealGlobal* unrealGlobal = UnrealGlobal::GetInstance();
// 	unrealGlobal->InitAll(GetWorld());
// }

// void AGlobalActor::CreateUI() {
// 	if (!CanvasTextWidget) {
// 		UE_LOG(LogTemp, Warning, TEXT("GlobalActor.CreateUI missing CanvasTextWidget"));
// 	} else {
// 		CanvasTextWidget->AddToViewport(0);
// 		// CanvasTextWidget->SetText(Lodash::GetInstanceId("TextHere_"));
// 		CanvasTextWidget->SetText(Lodash::GetInstanceId(""));
// 		UnrealGlobal* unrealGlobal = UnrealGlobal::GetInstance();
// 		unrealGlobal->SetWidgets(CanvasTextWidget);
// 	}
// }

void AGlobalActor::LisbonExteriors() {
	// CreateUI();
	UE_LOG(LogTemp, Display, TEXT("LisbonExteriors"));
	// Init();
	// TODO - not working well; just doing all with sequencer..
	LisbonSequence* lisbonSequence = LisbonSequence::GetInstance();
	lisbonSequence->Start();
}

void AGlobalActor::LisbonWorld() {
	UE_LOG(LogTemp, Display, TEXT("LisbonWorldIntro"));
	// MovePolyLine* movePolyLine = MovePolyLine::GetInstance();
	// movePolyLine->SetWorld(GetWorld());
	// movePolyLine->CleanUp();
	// // float z = -25;
	// // TArray<FVector> vertices = { FVector(0,0,0+z), FVector(10,0,10+z), FVector(20,0,15+z), FVector(30,0,10+z), FVector(40,0,0+z) };
	// TArray<FVector> vertices = MathVector::BeizerCurvePoints(FVector(0,0,0), FVector(40,0,0), FVector(20,0,15), 5);
	// movePolyLine->Move("key1", vertices, LineActorTemplate, "red", 10);

	LisbonWorldIntro* lisbonWorldIntro = LisbonWorldIntro::GetInstance();
	lisbonWorldIntro->Cables(GetWorld(), LineActorTemplate);

	if (!CanvasTextWidget) {
		UE_LOG(LogTemp, Warning, TEXT("GlobalActor.CreateUI missing CanvasTextWidget"));
	} else {
		CanvasTextWidget->AddToViewport(0);
		// CanvasTextWidget->SetText(Lodash::GetInstanceId("TextHere_"));
		// lisbonWorldIntro->UI(CanvasTextWidget);
		_world = GetWorld();
		// _step = 0;
		LisbonWorldUI();
	}
}

void AGlobalActor::LisbonWorldReScale() {
	LisbonWorldIntro* lisbonWorldIntro = LisbonWorldIntro::GetInstance();
	lisbonWorldIntro->ReScaleCables();
}

void AGlobalActor::Test() {
	MovePolyLine* movePolyLine = MovePolyLine::GetInstance();
	movePolyLine->SetWorld(GetWorld());
	movePolyLine->CleanUp();
	// TArray<FVector> vertices = { FVector(0,0,0), FVector(10,0,10), FVector(20,0,15), FVector(30,0,10), FVector(40,0,0) };
	TArray<FVector> vertices = MathVector::BeizerCurvePoints(FVector(0,0,0), FVector(40,0,0), FVector(20,0,15), 5);
	FString key = Lodash::GetInstanceId("key1");
	// movePolyLine->Move("key1", vertices, LineActorTemplate, "red", 10);
	movePolyLine->CreateActors(key, vertices, LineActorTemplate, "red");
	for (int ii = 0; ii < vertices.Num() - 1; ii++) {
		FString actorName = movePolyLine->FormName(key, ii);
		if (movePolyLine->_actors.Contains(actorName)) {
			AActor* actor = movePolyLine->_actors[actorName];
			float currentMaxScaleX = movePolyLine->GetLineMaxXScale(vertices[ii], vertices[ii + 1]);
			FVector scale = FVector(currentMaxScaleX, 1, 1);
			actor->SetActorScale3D(scale);
		}
	}

	// Default point (Lisbon data center) 38.988267, -8.965344
	float zGround = 10;
	TArray<TArray<FVector>> cables = {
		// Canary Islands, Spain 28.0050668,-15.7003936 -> Sexial, Portugal 38.6368383,-9.2077008
		{ FVector(58227775 / 100, -122702638 / 100, zGround), FVector(2100120 / 100, -3904642 / 100, zGround) },
		// Canary Islands, Spain 28.0050668,-15.7003936 -> Carcavelos, Portugal
		// TODO
	};
	FVector control, start, end;
	float zControl = 1000000;
	float len;
	int numPoints = 100;
	float zScale = 1000;
	for (int cc = 0; cc < cables.Num(); cc++) {
		start = cables[cc][0];
		end = cables[cc][1];
		len = (end - start).Size();
		control = start + (end - start).GetClampedToMaxSize(len * 0.5);
		control.Z = zControl;
		vertices = MathVector::BeizerCurvePoints(start, end, control, numPoints);
		key = Lodash::GetInstanceId("cable_");

		movePolyLine->CreateActors(key, vertices, LineActorTemplate, "red");
		for (int ii = 0; ii < vertices.Num() - 1; ii++) {
			FString actorName = movePolyLine->FormName(key, ii);
			if (movePolyLine->_actors.Contains(actorName)) {
				AActor* actor = movePolyLine->_actors[actorName];
				float currentMaxScaleX = movePolyLine->GetLineMaxXScale(vertices[ii], vertices[ii + 1]);
				FVector scale = FVector(currentMaxScaleX, zScale, zScale);
				actor->SetActorScale3D(scale);
			}
		}
	}
}

void AGlobalActor::LisbonWorldUI(int step) {
	// UE_LOG(LogTemp, Display, TEXT("step %d"), step);
	if (!_world) {
		UE_LOG(LogTemp, Warning, TEXT("LisbonWorldIntro.UI world not set, skipping"));
	} else {
		if (step == 0) {
			// _step = step + 1;
			FSlateFontInfo fontInfo = CanvasTextWidget->GetFont();
			fontInfo.Size = 80;
			CanvasTextWidget->SetFont(fontInfo);
			CanvasTextWidget->SetText("Edged Lisbon");
			_uiDelegate.BindUFunction(this, "LisbonWorldUI", (step + 1));
			_world->GetTimerManager().SetTimer(_uiTimer, _uiDelegate, 5, false);
		} else if (step == 1) {
			// _step = step + 1;
			CanvasTextWidget->SetText("");
			_uiDelegate.BindUFunction(this, "LisbonWorldUI", (step + 1));
			_world->GetTimerManager().SetTimer(_uiTimer, _uiDelegate, 10, false);
		} else if (step == 2) {
			// _step = step + 1;
			FSlateFontInfo fontInfo = CanvasTextWidget->GetFont();
			fontInfo.Size = 40;
			CanvasTextWidget->SetFont(fontInfo);
			CanvasTextWidget->SetText("Europe's highest capacity fiber crossroads is now the site for Portugal's greenest data center.");
			_uiDelegate.BindUFunction(this, "LisbonWorldUI", (step + 1));
			_world->GetTimerManager().SetTimer(_uiTimer, _uiDelegate, 10, false);
		} else if (step == 3) {
			// _step = step + 1;
			CanvasTextWidget->SetText("");
		}
	}
}
