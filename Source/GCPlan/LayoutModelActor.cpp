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
}

void ALayoutModelActor::DestroyActors() {
	Init();
	ModelBase* modelBase = ModelBase::GetInstance();
	if (modelBase) modelBase->DestroyActors();//smm230623//
	LayoutModelBase* layoutModelBase = LayoutModelBase::GetInstance();
	if (layoutModelBase) layoutModelBase->DestroyActors();//smm230623//
}

