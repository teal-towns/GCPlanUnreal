#include "GlobalActor.h"

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

void AGlobalActor::LisbonExteriors() {
	UE_LOG(LogTemp, Display, TEXT("LisbonExteriors"));
	// Init();
	LisbonSequence* lisbonSequence = LisbonSequence::GetInstance();
	lisbonSequence->Start();
}
