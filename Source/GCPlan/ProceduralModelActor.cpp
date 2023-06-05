#include "ProceduralModelActor.h"

#include "Modeling/ModelBase.h"
#include "ProceduralModel/PMBase.h"

AProceduralModelActor::AProceduralModelActor()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AProceduralModelActor::BeginPlay()
{
	Super::BeginPlay();	
}

// void AProceduralModelActor::Tick(float DeltaTime)
// {
// 	Super::Tick(DeltaTime);
// }

void AProceduralModelActor::Init() {
	ModelBase* modelBase = ModelBase::GetInstance();
    modelBase->SetWorld(GetWorld());

    PMBase* pmBase = PMBase::GetInstance();
    pmBase->SetWorld(GetWorld());
}

void AProceduralModelActor::GenerateModel() {
	Init();
	PMBase* pmBase = PMBase::GetInstance();
	pmBase->SetInputs(ProceduralModelBase);
    pmBase->Create();
}