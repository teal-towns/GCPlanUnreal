#include "GlobalActor.h"

// #include "CanvasTextWidget.h"
#include "Common/Lodash.h"
#include "Common/UnrealGlobal.h"
#include "Move/MoveObject.h"
#include "XLisbon/LisbonSequence.h"

AGlobalActor::AGlobalActor()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AGlobalActor::BeginPlay()
{
	Super::BeginPlay();	
}

void AGlobalActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	MoveObject* moveObject = MoveObject::GetInstance();
	moveObject->Tick(DeltaTime);
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
