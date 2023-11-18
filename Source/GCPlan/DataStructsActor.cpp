#include "DataStructsActor.h"

#include "BuildingStructsActor.h"

ADataStructsActor::ADataStructsActor()
{
	PrimaryActorTick.bCanEverTick = false;
}

FDataProjectJsonSimplified ADataStructsActor::ProjectToSimplified(FDataProjectJson data) {
	return FDataProjectJsonSimplified(ABuildingStructsActor::PolygonsToSimplified(data.polygons));
}

FDataProjectJson ADataStructsActor::ProjectFromSimplified(FDataProjectJsonSimplified dataSimplified) {
	return FDataProjectJson(ABuildingStructsActor::PolygonsFromSimplified(dataSimplified.polygons));
}
