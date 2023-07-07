#pragma once

#include <mutex>

#include "../BuildingStructsActor.h"
#include "../Modeling/ModelBase.h"

struct FCondenser {
	FVector offset = FVector(0,0,0);
	FVector rotation = FVector(0,0,0);
};

class LisbonExteriors {
public:
	LisbonExteriors();
	~LisbonExteriors();

	// Singletons should not be cloneable or assignable.
	LisbonExteriors(LisbonExteriors &other) = delete;
	void operator=(const LisbonExteriors &) = delete;

	static LisbonExteriors *GetInstance();

	static void Buildings(float zOffset = 0);
	static TMap<FString, FPolygon> BuildingCondensers(FVector size, FModelParams modelParams,
		FModelCreateParams createParamsIn = FModelCreateParams());
	static TMap<FString, FPolygon> CondenserGroup(FVector size, FModelParams modelParams,
		FModelCreateParams createParamsIn = FModelCreateParams(), FCondenser params = FCondenser());
	static TMap<FString, FPolygon> ParkingLots(float zOffset = 0);

private:
	static LisbonExteriors *pinstance_;
	static std::mutex mutex_;
};
