#include "LayoutModelActor.h"

#include "Common/UnrealGlobal.h"
#include "LayoutModel/LayoutModelBase.h"
#include "Modeling/ModelBase.h"

ALayoutModelActor::ALayoutModelActor()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ALayoutModelActor::BeginPlay()
{
	Super::BeginPlay();	
}

// void ALayoutModelActor::Tick(float DeltaTime)
// {
// 	Super::Tick(DeltaTime);
// }

void ALayoutModelActor::Init() {
	UnrealGlobal* unrealGlobal = UnrealGlobal::GetInstance();
	unrealGlobal->InitAll(GetWorld(), { "web" });
}

void ALayoutModelActor::GenerateModel() {
	Init();
	LayoutModelBase* layoutModelBase = LayoutModelBase::GetInstance();
	layoutModelBase->SetInputs(LayoutParams);
	layoutModelBase->Create();
	// ModelBase* modelBase = ModelBase::GetInstance();
	// modelBase->CreateFloor();
}

void ALayoutModelActor::DestroyActors() {
	Init();
	ModelBase* modelBase = ModelBase::GetInstance();
	modelBase->DestroyActors();
	LayoutModelBase* layoutModelBase = LayoutModelBase::GetInstance();
	layoutModelBase->DestroyActors();
}

