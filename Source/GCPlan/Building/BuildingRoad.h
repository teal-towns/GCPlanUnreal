#pragma once

#include "../BuildingStructsActor.h"

class BuildingRoad {
public:
	BuildingRoad();
	~BuildingRoad();

	static TMap<FString, FRoadPath> BetweenSpaces(TArray<TArray<FVector>> spacesVertices,
		float verticesBuffer);
};
