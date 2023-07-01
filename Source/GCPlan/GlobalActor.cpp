#include "GlobalActor.h"

#include "Fonts/SlateFontInfo.h"

#include "CanvasTextWidget.h"
#include "Common/Lodash.h"
#include "Common/MathVector.h"
#include "Common/UnrealGlobal.h"
#include "Draw/DrawHighlight.h"
#include "Modeling/ModelBase.h"
#include "Modeling/Common/ModelHighlight.h"
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
	// lisbonWorldIntro->Text3D();
	_world = GetWorld();
	LisbonWorldText3D();

	if (!CanvasTextWidget) {
		UE_LOG(LogTemp, Warning, TEXT("GlobalActor.CreateUI missing CanvasTextWidget"));
	} else {
		CanvasTextWidget->AddToViewport(0);
		// CanvasTextWidget->SetText(Lodash::GetInstanceId("TextHere_"));
		// lisbonWorldIntro->UI(CanvasTextWidget);
		// _step = 0;
		LisbonWorldUI();
	}
}

void AGlobalActor::LisbonWorldReScale() {
	LisbonWorldIntro* lisbonWorldIntro = LisbonWorldIntro::GetInstance();
	lisbonWorldIntro->ReScaleCables();
}

void AGlobalActor::Test() {
	// // CABLES
	// MovePolyLine* movePolyLine = MovePolyLine::GetInstance();
	// movePolyLine->SetWorld(GetWorld());
	// movePolyLine->CleanUp();
	// // TArray<FVector> vertices = { FVector(0,0,0), FVector(10,0,10), FVector(20,0,15), FVector(30,0,10), FVector(40,0,0) };
	// TArray<FVector> vertices = MathVector::BeizerCurvePoints(FVector(0,0,0), FVector(40,0,0), FVector(20,0,15), 5);
	// FString key = Lodash::GetInstanceId("key1");
	// // movePolyLine->Move("key1", vertices, LineActorTemplate, "red", 10);
	// movePolyLine->CreateActors(key, vertices, LineActorTemplate, "red");
	// for (int ii = 0; ii < vertices.Num() - 1; ii++) {
	// 	FString actorName = movePolyLine->FormName(key, ii);
	// 	if (movePolyLine->_actors.Contains(actorName)) {
	// 		AActor* actor = movePolyLine->_actors[actorName];
	// 		float currentMaxScaleX = movePolyLine->GetLineMaxXScale(vertices[ii], vertices[ii + 1]);
	// 		FVector scale = FVector(currentMaxScaleX, 1, 1);
	// 		actor->SetActorScale3D(scale);
	// 	}
	// }

	// // Default point (Lisbon data center) 38.988267, -8.965344
	// float zGround = 10;
	// TArray<TArray<FVector>> cables = {
	// 	// Canary Islands, Spain 28.0050668,-15.7003936 -> Sexial, Portugal 38.6368383,-9.2077008
	// 	{ FVector(58227775 / 100, -122702638 / 100, zGround), FVector(2100120 / 100, -3904642 / 100, zGround) },
	// 	// Canary Islands, Spain 28.0050668,-15.7003936 -> Carcavelos, Portugal
	// 	// TODO
	// };
	// FVector control, start, end;
	// float zControl = 1000000;
	// float len;
	// int numPoints = 100;
	// float zScale = 1000;
	// for (int cc = 0; cc < cables.Num(); cc++) {
	// 	start = cables[cc][0];
	// 	end = cables[cc][1];
	// 	len = (end - start).Size();
	// 	control = start + (end - start).GetClampedToMaxSize(len * 0.5);
	// 	control.Z = zControl;
	// 	vertices = MathVector::BeizerCurvePoints(start, end, control, numPoints);
	// 	key = Lodash::GetInstanceId("cable_");

	// 	movePolyLine->CreateActors(key, vertices, LineActorTemplate, "red");
	// 	for (int ii = 0; ii < vertices.Num() - 1; ii++) {
	// 		FString actorName = movePolyLine->FormName(key, ii);
	// 		if (movePolyLine->_actors.Contains(actorName)) {
	// 			AActor* actor = movePolyLine->_actors[actorName];
	// 			float currentMaxScaleX = movePolyLine->GetLineMaxXScale(vertices[ii], vertices[ii + 1]);
	// 			FVector scale = FVector(currentMaxScaleX, zScale, zScale);
	// 			actor->SetActorScale3D(scale);
	// 		}
	// 	}
	// }

	// 3D TEXT
	UnrealGlobal* unrealGlobal = UnrealGlobal::GetInstance();
	TArray<FString> skipKeys = { "web" };
	unrealGlobal->InitAll(GetWorld(), skipKeys);

	FModelCreateParams createParams;
	createParams.offset = FVector(-15000, 28000, 0);
	createParams.rotation = FVector(0,0,295);
	FHighlightText paramsText;
	DrawHighlight* drawHighlight = DrawHighlight::GetInstance();
	paramsText.dotScale = FVector(30,30,1);
	paramsText.dotMeshKey = "cylinder";
	// paramsText.dotRotation = FVector(0,90,0);
	paramsText.textOffset = FVector(20,20,0);
	paramsText.textRotation = FVector(0,-90,0);
	paramsText.subTextOffset = FVector(-20,20,0);
	FVector scaleLarge = FVector(250,250,250);
	FVector scale = FVector(75,75,75);
	paramsText.text = "Lisbon";
	paramsText.subText = "Up to 100 MW";
	// FVector scale = FVector(1,100,100);
	drawHighlight->CreateOne("lisbon", scale, FModelParams(), createParams, {}, paramsText);
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

void AGlobalActor::LisbonWorldText3D(int step) {
	if (!_world) {
		UE_LOG(LogTemp, Warning, TEXT("LisbonWorldText3D world not set, skipping"));
	} else {
		FModelCreateParams createParams;
		createParams.rotation = FVector(0,0,270);
		FHighlightText paramsText;
		paramsText.dotScale = FVector(50,50,1);
		paramsText.dotMeshKey = "cylinder";
		// paramsText.dotRotation = FVector(0,90,0);
		paramsText.textOffset = FVector(40,40,0);
		paramsText.textRotation = FVector(0,-90,0);
		paramsText.subTextOffset = FVector(-40,40,0);
		FVector scaleLarge = FVector(250,250,250);
		FVector scale = FVector(100,100,100);
		FModelParams modelParams;
		modelParams.movable = true;
		// float zOffset = 500000;
		float zOffset = 100000;
		DrawHighlight* drawHighlight = DrawHighlight::GetInstance();
		if (step == 0) {
			// First is to wait.
			_text3DDelegate.BindUFunction(this, "LisbonWorldText3D", (step + 1));
			_world->GetTimerManager().SetTimer(_text3DTimer, _text3DDelegate, 7, false);
		} else if (step == 1) {
			createParams.offset = FVector(840600, -301600, zOffset);
			paramsText.text = "Barcelona";
			paramsText.subText = "Up to 16 MW";
			paramsText.textAlignment = "right";
			paramsText.textOffset = FVector(80, -40, 0);
			paramsText.subTextOffset = FVector(0, -40, 0);
			drawHighlight->CreateOne("barcelona", scaleLarge, modelParams, createParams, {}, paramsText);
			_text3DDelegate.BindUFunction(this, "LisbonWorldText3D", (step + 1));
			_world->GetTimerManager().SetTimer(_text3DTimer, _text3DDelegate, 1, false);
		} else if (step == 2) {
			createParams.offset = FVector(473600, -464300, zOffset);
			paramsText.text = "Bilbao Arasur";
			paramsText.subText = "Up to 100 MW";
			paramsText.textAlignment = "right";
			paramsText.textOffset = FVector(-150, -40, 0);
			paramsText.subTextOffset = FVector(-230, -40, 0);
			drawHighlight->CreateOne("bilbao", scaleLarge, modelParams, createParams, {}, paramsText);
			_text3DDelegate.BindUFunction(this, "LisbonWorldText3D", (step + 1));
			_world->GetTimerManager().SetTimer(_text3DTimer, _text3DDelegate, 1, false);
		} else if (step == 3) {
			createParams.offset = FVector(442420, -199000, zOffset);
			paramsText.text = "Madrid Getafe";
			paramsText.subText = "Up to 20 MW";
			paramsText.textOffset = FVector(-190, -450, 0);
			paramsText.subTextOffset = FVector(-270, -450, 0);
			drawHighlight->CreateOne("madrid", scaleLarge, modelParams, createParams, {}, paramsText);
			_text3DDelegate.BindUFunction(this, "LisbonWorldText3D", (step + 1));
			_world->GetTimerManager().SetTimer(_text3DTimer, _text3DDelegate, 1, false);
		} else if (step == 4) {
			createParams.offset = FVector(-1000, 1000, zOffset);
			paramsText.text = "Lisbon";
			paramsText.subText = "Up to 100 MW";
			drawHighlight->CreateOne("lisbon", scaleLarge, modelParams, createParams, {}, paramsText);
			_text3DDelegate.BindUFunction(this, "LisbonWorldText3D", (step + 1));
			_world->GetTimerManager().SetTimer(_text3DTimer, _text3DDelegate, 8, false);
		} else if (step == 5) {
			drawHighlight->DestroyOne("barcelona");
			drawHighlight->DestroyOne("bilbao");
			drawHighlight->DestroyOne("madrid");
			drawHighlight->DestroyOne("lisbon");

			_text3DDelegate.BindUFunction(this, "LisbonWorldText3D", (step + 1));
			_world->GetTimerManager().SetTimer(_text3DTimer, _text3DDelegate, 1, false);
		} else if (step == 6) {
			createParams.offset = FVector(-10523, 38913, 10000);
			paramsText.text = "Sexial";
			drawHighlight->CreateOne("sexial", scale, modelParams, createParams, {}, paramsText);
			_text3DDelegate.BindUFunction(this, "LisbonWorldText3D", (step + 1));
			_world->GetTimerManager().SetTimer(_text3DTimer, _text3DDelegate, 1, false);
		} else if (step == 7) {
			createParams.offset = FVector(-11768, 58054, 10000);
			paramsText.text = "Sesimbra";
			drawHighlight->CreateOne("sesimbra", scale, modelParams, createParams, {}, paramsText);
			_text3DDelegate.BindUFunction(this, "LisbonWorldText3D", (step + 1));
			_world->GetTimerManager().SetTimer(_text3DTimer, _text3DDelegate, 1, false);
		} else if (step == 8) {
			createParams.offset = FVector(-33489, 31478, 10000);
			paramsText.text = "Carcavelos";
			drawHighlight->CreateOne("carcavelos", scale, modelParams, createParams, {}, paramsText);
			_text3DDelegate.BindUFunction(this, "LisbonWorldText3D", (step + 1));
			_world->GetTimerManager().SetTimer(_text3DTimer, _text3DDelegate, 1, false);
		}
	}
}
