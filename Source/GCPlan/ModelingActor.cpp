#include "ModelingActor.h"

#include "Common/UnrealGlobal.h"
#include "Modeling/ModelBase.h"
// #include "ProceduralModel/PMBase.h"

AModelingActor::AModelingActor()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AModelingActor::BeginPlay()
{
	Super::BeginPlay();	
}

// void AModelingActor::Tick(float DeltaTime)
// {
// 	Super::Tick(DeltaTime);
// }

void AModelingActor::Init() {
	UnrealGlobal* unrealGlobal = UnrealGlobal::GetInstance();
	unrealGlobal->InitAll(GetWorld(), { "web" });
	// ModelBase* modelBase = ModelBase::GetInstance();
	// modelBase->SetWorld(GetWorld());

	// PMBase* pmBase = PMBase::GetInstance();
	// pmBase->SetWorld(GetWorld());
}

void AModelingActor::GenerateModel() {
	Init();
	ModelBase* modelBase = ModelBase::GetInstance();
	modelBase->SetInputs(ModelingBase);
	modelBase->Create();
}

void AModelingActor::CreateFloor() {
	Init();
	ModelBase* modelBase = ModelBase::GetInstance();
	modelBase->CreateFloor();
}

void AModelingActor::DestroyActors() {
	Init();
	ModelBase* modelBase = ModelBase::GetInstance();
	modelBase->DestroyActors();
}
