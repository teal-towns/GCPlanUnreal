#pragma once

class BuildingRoad {
public:
	BuildingRoad();
	~BuildingRoad();

	static TMap<FString, TArray<FVector>> BetweenSpaces(TArray<TArray<FVector>> spacesVerticesBuffered);
};
