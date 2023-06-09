#include "DataStructsActor.h"

#include "BuildingStructsActor.h"

ADataStructsActor::ADataStructsActor()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ADataStructsActor::BeginPlay()
{
	Super::BeginPlay();	
}

// void ADataStructsActor::Tick(float DeltaTime)
// {
// 	Super::Tick(DeltaTime);
// }

FDataProjectJsonSimplified ADataStructsActor::ProjectToSimplified(FDataProjectJson data) {
	return FDataProjectJsonSimplified(ABuildingStructsActor::PlotsToSimplified(data.plots));
}

FDataProjectJson ADataStructsActor::ProjectFromSimplified(FDataProjectJsonSimplified dataSimplified) {
	return FDataProjectJson(ABuildingStructsActor::PlotsFromSimplified(dataSimplified.plots));
}
