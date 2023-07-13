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
	DrawHighlight* drawHighlight = DrawHighlight::GetInstance();
	drawHighlight->Tick(DeltaTime);
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

// void AGlobalActor::LisbonWorldReScale() {
// 	LisbonWorldIntro* lisbonWorldIntro = LisbonWorldIntro::GetInstance();
// 	lisbonWorldIntro->ReScaleCables();
// }

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
		FString animateInFunction = "SlideInFadeIn";
		// FString animateInFunction = "AnimateTextLetters";
		FString animateOutFunction = "FadeOut";
		if (step == 0) {
			CanvasTextWidget->SetImage(0.8);
			CanvasTextWidget->PlayMovie();
			_uiDelegate.BindUFunction(this, "LisbonWorldUI", (step + 1));
			_world->GetTimerManager().SetTimer(_uiTimer, _uiDelegate, 3, false);
		} else if (step == 1) {
			CanvasTextWidget->SetImageAnimate("BackgroundImageFadeOut");
			FSlateFontInfo fontInfo = CanvasTextWidget->GetFont();
			fontInfo.Size = 80;
			CanvasTextWidget->SetFont(fontInfo);
			CanvasTextWidget->SetText("Edged Lisbon", "ScaleInFadeIn", 4, animateOutFunction);
			_uiDelegate.BindUFunction(this, "LisbonWorldUI", (step + 1));
			_world->GetTimerManager().SetTimer(_uiTimer, _uiDelegate, 1, false);
		} else if (step == 2) {
			CanvasTextWidget->HideMovie();
			_uiDelegate.BindUFunction(this, "LisbonWorldUI", (step + 1));
			_world->GetTimerManager().SetTimer(_uiTimer, _uiDelegate, 4, false);
		} else if (step == 3) {
			CanvasTextWidget->SetText("");
			_uiDelegate.BindUFunction(this, "LisbonWorldUI", (step + 1));
			_world->GetTimerManager().SetTimer(_uiTimer, _uiDelegate, 5, false);
		} else if (step == 4) {
			_uiDelegate.BindUFunction(this, "LisbonWorldUI", (step + 1));
			_world->GetTimerManager().SetTimer(_uiTimer, _uiDelegate, 5, false);
		} else if (step == 5) {
			// _step = step + 1;
			FSlateFontInfo fontInfo = CanvasTextWidget->GetFont();
			fontInfo.Size = 40;
			CanvasTextWidget->SetFont(fontInfo);
			CanvasTextWidget->SetText("Europe's highest capacity fiber crossroads is now the site for Portugal's greenest data center.",
				"AnimateTextLetters", 4, animateOutFunction, 0.01, 3);
			_uiDelegate.BindUFunction(this, "LisbonWorldUI", (step + 1));
			_world->GetTimerManager().SetTimer(_uiTimer, _uiDelegate, 5, false);
		} else if (step == 6) {
			CanvasTextWidget->SetText("");
			_uiDelegate.BindUFunction(this, "LisbonWorldUI", (step + 1));
			_world->GetTimerManager().SetTimer(_uiTimer, _uiDelegate, 5, false);
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

		// FDrawHighlight drawParams;
		// drawParams.scaleSpeed = 10;
		FVector Text3DScaleSmall = FVector(10,10,10);
		float scaleSpeedFade = 10;

		FVector scaleLarge = FVector(250,250,250);
		FVector scale = FVector(70,70,70);
		FVector dotScaleSmall = FVector(30,30,1);
		FModelParams modelParams;
		modelParams.movable = true;
		// float zOffset = 500000;
		float zOffset = 100000;
		DrawHighlight* drawHighlight = DrawHighlight::GetInstance();
		LisbonWorldIntro* lisbonWorldIntro = LisbonWorldIntro::GetInstance();
		MovePolyLine* movePolyLine = MovePolyLine::GetInstance();

		TArray<FVector> vertices;
		int numPoints = 50;
		// TArray<FString> colors = { "blueEmissive", "greenEmissive", "orangeEmissive", "purpleEmissive", "redEmissive", "yellowEmissive" };
		// int colorsCount = colors.Num();
		float moveSpeed = 100000;
		float zScale = 1000;
		FVector control;
		float zGround = 10000;
		float zGroundTextBuffer = 10000;
		FVector start = FVector(0,0,zGround);

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
			_world->GetTimerManager().SetTimer(_text3DTimer, _text3DDelegate, 3, false);
		} else if (step == 5) {
			movePolyLine->ReScale(500, 1000);
			_text3DDelegate.BindUFunction(this, "LisbonWorldText3D", (step + 1));
			_world->GetTimerManager().SetTimer(_text3DTimer, _text3DDelegate, 3, false);
		} else if (step == 6) {
			// drawHighlight->UpdateTargetScale("barcelona", Text3DScaleSmall, scaleSpeedFade);
			// drawHighlight->UpdateTargetScale("bilbao", Text3DScaleSmall, scaleSpeedFade);
			// drawHighlight->UpdateTargetScale("madrid", Text3DScaleSmall, scaleSpeedFade);
			// drawHighlight->UpdateTargetScale("lisbon", Text3DScaleSmall, scaleSpeedFade);
			_text3DDelegate.BindUFunction(this, "LisbonWorldText3D", (step + 1));
			_world->GetTimerManager().SetTimer(_text3DTimer, _text3DDelegate, 2, false);
		} else if (step == 7) {
			drawHighlight->DestroyOne("barcelona");
			drawHighlight->DestroyOne("bilbao");
			drawHighlight->DestroyOne("madrid");
			drawHighlight->DestroyOne("lisbon");

			_text3DDelegate.BindUFunction(this, "LisbonWorldText3D", (step + 1));
			_world->GetTimerManager().SetTimer(_text3DTimer, _text3DDelegate, 1, false);
		} else if (step == 8) {
			createParams.offset = FVector(-10523, 38913, zGround + zGroundTextBuffer);
			paramsText.text = "Sexial";
			paramsText.dotScale = dotScaleSmall;
			drawHighlight->CreateOne("sexial", scale, modelParams, createParams, {}, paramsText);

			// control = FVector(8645, 23612, zGround);
			// vertices = MathVector::BeizerCurvePoints(start, createParams.offset,
			// 	control, numPoints);
			vertices = { start, FVector(6000, 12667, zGround), FVector(7000, 26339, zGround),
				FVector(-1895, 36505, zGround), createParams.offset
			};
			vertices = lisbonWorldIntro->PointsToSquigglePath(vertices);
			movePolyLine->Move(Lodash::GetInstanceId("sexial"), vertices, LineActorTemplate, "blueEmissive",
				moveSpeed, FVector(0,zScale,zScale));

			_text3DDelegate.BindUFunction(this, "LisbonWorldText3D", (step + 1));
			_world->GetTimerManager().SetTimer(_text3DTimer, _text3DDelegate, 1, false);
		} else if (step == 9) {
			createParams.offset = FVector(-11768, 58054, zGround + zGroundTextBuffer);
			paramsText.text = "Sesimbra";
			paramsText.dotScale = dotScaleSmall;
			drawHighlight->CreateOne("sesimbra", scale, modelParams, createParams, {}, paramsText);

			// control = FVector(9666, 32344, zGround);
			// vertices = MathVector::BeizerCurvePoints(start, createParams.offset,
			// 	control, numPoints);
			vertices = { start, FVector(7920, 22483, zGround), FVector(1960, 38959, zGround),
				createParams.offset
			};
			vertices = lisbonWorldIntro->PointsToSquigglePath(vertices);
			movePolyLine->Move(Lodash::GetInstanceId("sesimbra"), vertices, LineActorTemplate, "greenEmissive",
				moveSpeed, FVector(0,zScale,zScale));

			_text3DDelegate.BindUFunction(this, "LisbonWorldText3D", (step + 1));
			_world->GetTimerManager().SetTimer(_text3DTimer, _text3DDelegate, 1, false);
		} else if (step == 10) {
			createParams.offset = FVector(-33489, 31478, zGround + zGroundTextBuffer);
			paramsText.text = "Carcavelos";
			paramsText.dotScale = dotScaleSmall;
			drawHighlight->CreateOne("carcavelos", scale, modelParams, createParams, {}, paramsText);

			// control = FVector(-13464, 23184, zGround);
			// vertices = MathVector::BeizerCurvePoints(start, createParams.offset,
			// 	control, numPoints);
			vertices = { start, FVector(-9608, 10914, zGround), FVector(-12763, 25638, zGround),
				createParams.offset
			};
			vertices = lisbonWorldIntro->PointsToSquigglePath(vertices);
			movePolyLine->Move(Lodash::GetInstanceId("carcavelos"), vertices, LineActorTemplate, "yellowEmissive",
				moveSpeed, FVector(0,zScale,zScale));

			_text3DDelegate.BindUFunction(this, "LisbonWorldText3D", (step + 1));
			_world->GetTimerManager().SetTimer(_text3DTimer, _text3DDelegate, 5, false);
		} else if (step == 11) {
			movePolyLine->ReScale(0, 250);
			// drawHighlight->UpdateTargetScale("sexial", Text3DScaleSmall, scaleSpeedFade);
			// drawHighlight->UpdateTargetScale("sesimbra", Text3DScaleSmall, scaleSpeedFade);
			// drawHighlight->UpdateTargetScale("carcavelos", Text3DScaleSmall, scaleSpeedFade);
			drawHighlight->DestroyOne("sexial");
			drawHighlight->DestroyOne("sesimbra");
			drawHighlight->DestroyOne("carcavelos");
		}
	}
}

void AGlobalActor::LisbonInteriorsStart() {
	_world = GetWorld();
	if (!CanvasTextWidget) {
		UE_LOG(LogTemp, Warning, TEXT("GlobalActor.LisbonInteriorsStart missing CanvasTextWidget"));
	} else {
		CanvasTextWidget->AddToViewport(0);
		LisbonInteriors();
	}
}

void AGlobalActor::LisbonInteriors(int step) {
	// 90 - 300 - 510 - 720 - 930 - 1140 - 1350
	FString animateInFunction = "SlideInFadeIn";
	// FString animateInFunction = "AnimateTextLetters";
	FString animateOutFunction = "FadeOut";
	if (step == 0) {
		CanvasTextWidget->SetText("LOBBY", animateInFunction, 4, animateOutFunction);
		_interiorsDelegate.BindUFunction(this, "LisbonInteriors", (step + 1));
		_world->GetTimerManager().SetTimer(_interiorsTimer, _interiorsDelegate, 5, false);
	} else if (step == 1) {
		CanvasTextWidget->SetText("");
		_interiorsDelegate.BindUFunction(this, "LisbonInteriors", (step + 1));
		_world->GetTimerManager().SetTimer(_interiorsTimer, _interiorsDelegate, 2, false);
	} else if (step == 2) {
		CanvasTextWidget->SetText("KITCHEN", animateInFunction, 4, animateOutFunction);
		_interiorsDelegate.BindUFunction(this, "LisbonInteriors", (step + 1));
		_world->GetTimerManager().SetTimer(_interiorsTimer, _interiorsDelegate, 5, false);
	} else if (step == 3) {
		CanvasTextWidget->SetText("");
		_interiorsDelegate.BindUFunction(this, "LisbonInteriors", (step + 1));
		_world->GetTimerManager().SetTimer(_interiorsTimer, _interiorsDelegate, 2, false);
	} else if (step == 4) {
		CanvasTextWidget->SetText("OPERATIONS CENTER", animateInFunction, 4, animateOutFunction);
		_interiorsDelegate.BindUFunction(this, "LisbonInteriors", (step + 1));
		_world->GetTimerManager().SetTimer(_interiorsTimer, _interiorsDelegate, 5, false);
	} else if (step == 5) {
		CanvasTextWidget->SetText("");
		_interiorsDelegate.BindUFunction(this, "LisbonInteriors", (step + 1));
		_world->GetTimerManager().SetTimer(_interiorsTimer, _interiorsDelegate, 2, false);
	} else if (step == 6) {
		CanvasTextWidget->SetText("LOUNGE", animateInFunction, 4, animateOutFunction);
		_interiorsDelegate.BindUFunction(this, "LisbonInteriors", (step + 1));
		_world->GetTimerManager().SetTimer(_interiorsTimer, _interiorsDelegate, 5, false);
	} else if (step == 7) {
		CanvasTextWidget->SetText("");
		_interiorsDelegate.BindUFunction(this, "LisbonInteriors", (step + 1));
		_world->GetTimerManager().SetTimer(_interiorsTimer, _interiorsDelegate, 2, false);
	} else if (step == 8) {
		CanvasTextWidget->SetText("CONFERENCE ROOM", animateInFunction, 4, animateOutFunction);
		_interiorsDelegate.BindUFunction(this, "LisbonInteriors", (step + 1));
		_world->GetTimerManager().SetTimer(_interiorsTimer, _interiorsDelegate, 5, false);
	} else if (step == 9) {
		CanvasTextWidget->SetText("");
		_interiorsDelegate.BindUFunction(this, "LisbonInteriors", (step + 1));
		_world->GetTimerManager().SetTimer(_interiorsTimer, _interiorsDelegate, 2, false);
	} else if (step == 10) {
		CanvasTextWidget->SetText("DATA SERVERS", animateInFunction, 4, animateOutFunction);
		_interiorsDelegate.BindUFunction(this, "LisbonInteriors", (step + 1));
		_world->GetTimerManager().SetTimer(_interiorsTimer, _interiorsDelegate, 5, false);
	} else if (step == 11) {
		CanvasTextWidget->SetText("");
		_interiorsDelegate.BindUFunction(this, "LisbonInteriors", (step + 1));
		_world->GetTimerManager().SetTimer(_interiorsTimer, _interiorsDelegate, 2, false);
	}
}

void AGlobalActor::LisbonExteriorsTrainStart() {
	_world = GetWorld();
	if (!CanvasTextWidget) {
		UE_LOG(LogTemp, Warning, TEXT("GlobalActor.LisbonExteriorsTrainStart missing CanvasTextWidget"));
	} else {
		CanvasTextWidget->AddToViewport(0);
		LisbonExteriorsTrain();
	}
	// Init();
	// TODO - not working well; just doing all with sequencer..
	// LisbonSequence* lisbonSequence = LisbonSequence::GetInstance();
	// lisbonSequence->Start();
}

void AGlobalActor::LisbonExteriorsTrain(int step) {
	FString animateInFunction = "SlideInFadeIn";
	// FString animateInFunction = "AnimateTextLetters";
	FString animateOutFunction = "FadeOut";

	FModelCreateParams createParams;
	FHighlightText paramsText;
	paramsText.dotScale = FVector(1,1,1);
	FVector Text3DScale = FVector(3,3,3);
	FVector Text3DScaleSmall = FVector(0.1,0.1,0.1);
	FModelParams modelParams;
	modelParams.movable = true;
	DrawHighlight* drawHighlight = DrawHighlight::GetInstance();
	FDrawHighlight drawParams;
	drawParams.scaleSpeed = 10;

	if (step == 0) {
		// First is to wait.
		_exteriorsTrainDelegate.BindUFunction(this, "LisbonExteriorsTrain", (step + 1));
		_world->GetTimerManager().SetTimer(_exteriorsTrainTimer, _exteriorsTrainDelegate, 2, false);
	} else if (step == 1) {
		CanvasTextWidget->SetText("Accessible by rail", animateInFunction, 4, animateOutFunction);
		_exteriorsTrainDelegate.BindUFunction(this, "LisbonExteriorsTrain", (step + 1));
		_world->GetTimerManager().SetTimer(_exteriorsTrainTimer, _exteriorsTrainDelegate, 5, false);
	} else if (step == 2) {
		CanvasTextWidget->SetText("");
		_exteriorsTrainDelegate.BindUFunction(this, "LisbonExteriorsTrain", (step + 1));
		_world->GetTimerManager().SetTimer(_exteriorsTrainTimer, _exteriorsTrainDelegate, 2, false);
	} else if (step == 3) {
		CanvasTextWidget->SetText("Advanced zero water cooling system", animateInFunction, 4, animateOutFunction);

		createParams.offset = FVector(-216, -59, 24);
		createParams.rotation = FVector(0,0,-20);
		paramsText.text = "Waterless cooling";
		drawParams.scale = Text3DScaleSmall;
		drawParams.targetScale = Text3DScale;
		drawHighlight->CreateOne("rail", Text3DScale, modelParams, createParams, {}, paramsText, drawParams);

		_exteriorsTrainDelegate.BindUFunction(this, "LisbonExteriorsTrain", (step + 1));
		_world->GetTimerManager().SetTimer(_exteriorsTrainTimer, _exteriorsTrainDelegate, 5, false);
	} else if (step == 4) {
		drawParams.targetScale = Text3DScaleSmall;
		drawHighlight->UpdateTargetScale("rail", Text3DScaleSmall);
		CanvasTextWidget->SetText("");
		_exteriorsTrainDelegate.BindUFunction(this, "LisbonExteriorsTrain", (step + 1));
		_world->GetTimerManager().SetTimer(_exteriorsTrainTimer, _exteriorsTrainDelegate, 1, false);
	} else if (step == 5) {
		drawHighlight->DestroyOne("rail");
		FSlateFontInfo fontInfo = CanvasTextWidget->GetFont();
		fontInfo.Size = 80;
		CanvasTextWidget->SetFont(fontInfo);
		CanvasTextWidget->SetText("1.15 PUE", animateInFunction, 4, animateOutFunction);
		_exteriorsTrainDelegate.BindUFunction(this, "LisbonExteriorsTrain", (step + 1));
		_world->GetTimerManager().SetTimer(_exteriorsTrainTimer, _exteriorsTrainDelegate, 5, false);
	} else if (step == 4) {
		// Reset
		FSlateFontInfo fontInfo = CanvasTextWidget->GetFont();
		fontInfo.Size = 40;
		CanvasTextWidget->SetFont(fontInfo);
		CanvasTextWidget->SetText("");
		_exteriorsTrainDelegate.BindUFunction(this, "LisbonExteriorsTrain", (step + 1));
		_world->GetTimerManager().SetTimer(_exteriorsTrainTimer, _exteriorsTrainDelegate, 1, false);
	}
}

void AGlobalActor::LisbonExteriorsTwoStart() {
	_world = GetWorld();
	if (!CanvasTextWidget) {
		UE_LOG(LogTemp, Warning, TEXT("GlobalActor.LisbonExteriorsTwoStart missing CanvasTextWidget"));
	} else {
		CanvasTextWidget->AddToViewport(0);
		LisbonExteriorsTwo();
	}
}

void AGlobalActor::LisbonExteriorsTwo(int step) {
	FString animateInFunction = "SlideInFadeIn";
	// FString animateInFunction = "AnimateTextLetters";
	FString animateOutFunction = "FadeOut";

	FModelCreateParams createParams;
	FHighlightText paramsText;
	paramsText.dotScale = FVector(2,2,2);
	FVector Text3DScale = FVector(5,5,5);
	FVector Text3DScaleSmall = FVector(0.1,0.1,0.1);
	FModelParams modelParams;
	modelParams.movable = true;
	DrawHighlight* drawHighlight = DrawHighlight::GetInstance();
	FDrawHighlight drawParams;
	drawParams.scaleSpeed = 10;

	if (step == 0) {
		// First is to wait.
		_exteriorsTwoDelegate.BindUFunction(this, "LisbonExteriorsTwo", (step + 1));
		_world->GetTimerManager().SetTimer(_exteriorsTwoTimer, _exteriorsTwoDelegate, 1, false);
	} else if (step == 1) {
		CanvasTextWidget->SetText("High speed vehicle charging", animateInFunction, 4, animateOutFunction);

		createParams.offset = FVector(-206, -120, 22);
		createParams.rotation = FVector(0,-60,30);
		paramsText.text = "GridBlock EV Charging";
		drawParams.scale = Text3DScaleSmall;
		drawParams.targetScale = Text3DScale;
		drawHighlight->CreateOne("gridBlock", Text3DScale, modelParams, createParams, {}, paramsText, drawParams);

		_exteriorsTwoDelegate.BindUFunction(this, "LisbonExteriorsTwo", (step + 1));
		_world->GetTimerManager().SetTimer(_exteriorsTwoTimer, _exteriorsTwoDelegate, 5, false);
	} else if (step == 2) {
		drawParams.targetScale = Text3DScaleSmall;
		drawHighlight->UpdateTargetScale("gridBlock", Text3DScaleSmall);
		CanvasTextWidget->SetText("");
		_exteriorsTwoDelegate.BindUFunction(this, "LisbonExteriorsTwo", (step + 1));
		_world->GetTimerManager().SetTimer(_exteriorsTwoTimer, _exteriorsTwoDelegate, 2, false);
	} else if (step == 3) {
		drawHighlight->DestroyOne("gridBlock");

		createParams.offset = FVector(-206, -120, 22);
		createParams.rotation = FVector(0,-60,30);
		paramsText.text = "200 MWh battery storage";
		drawParams.scale = Text3DScaleSmall;
		drawParams.targetScale = Text3DScale;
		drawHighlight->CreateOne("batteryStorage", Text3DScale, modelParams, createParams, {}, paramsText, drawParams);

		_exteriorsTwoDelegate.BindUFunction(this, "LisbonExteriorsTwo", (step + 1));
		_world->GetTimerManager().SetTimer(_exteriorsTwoTimer, _exteriorsTwoDelegate, 4, false);
	} else if (step == 4) {
		drawParams.targetScale = Text3DScaleSmall;
		drawHighlight->UpdateTargetScale("batteryStorage", Text3DScaleSmall);

		_exteriorsTwoDelegate.BindUFunction(this, "LisbonExteriorsTwo", (step + 1));
		_world->GetTimerManager().SetTimer(_exteriorsTwoTimer, _exteriorsTwoDelegate, 1, false);
	} else if (step == 5) {
		drawHighlight->DestroyOne("batteryStorage");

		createParams.offset = FVector(-206, -120, 22);
		createParams.rotation = FVector(0,-60,30);
		paramsText.text = "Substation";
		drawParams.scale = Text3DScaleSmall;
		drawParams.targetScale = Text3DScale;
		drawHighlight->CreateOne("substation", Text3DScale, modelParams, createParams, {}, paramsText, drawParams);

		_exteriorsTwoDelegate.BindUFunction(this, "LisbonExteriorsTwo", (step + 1));
		_world->GetTimerManager().SetTimer(_exteriorsTwoTimer, _exteriorsTwoDelegate, 4, false);
	} else if (step == 6) {
		drawParams.targetScale = Text3DScaleSmall;
		drawHighlight->UpdateTargetScale("substation", Text3DScaleSmall);
		
		_exteriorsTwoDelegate.BindUFunction(this, "LisbonExteriorsTwo", (step + 1));
		_world->GetTimerManager().SetTimer(_exteriorsTwoTimer, _exteriorsTwoDelegate, 1, false);
	} else if (step == 7) {
		drawHighlight->DestroyOne("substation");
		CanvasTextWidget->SetText("Carbon neutral", animateInFunction, 4, animateOutFunction);
		
		_exteriorsTwoDelegate.BindUFunction(this, "LisbonExteriorsTwo", (step + 1));
		_world->GetTimerManager().SetTimer(_exteriorsTwoTimer, _exteriorsTwoDelegate, 5, false);
	} else if (step == 8) {
		CanvasTextWidget->SetText("");
		_exteriorsTwoDelegate.BindUFunction(this, "LisbonExteriorsTwo", (step + 1));
		_world->GetTimerManager().SetTimer(_exteriorsTwoTimer, _exteriorsTwoDelegate, 1, false);
	} else if (step == 9) {
		CanvasTextWidget->SetText("16,000 square meters premium office space in a pedestrian oriented campus",
			"AnimateTextLetters", 4, animateOutFunction, 0.01, 3);
		_exteriorsTwoDelegate.BindUFunction(this, "LisbonExteriorsTwo", (step + 1));
		_world->GetTimerManager().SetTimer(_exteriorsTwoTimer, _exteriorsTwoDelegate, 5, false);
	} else if (step == 10) {
		CanvasTextWidget->SetText("");
		_exteriorsTwoDelegate.BindUFunction(this, "LisbonExteriorsTwo", (step + 1));
		_world->GetTimerManager().SetTimer(_exteriorsTwoTimer, _exteriorsTwoDelegate, 1, false);
	}
}
