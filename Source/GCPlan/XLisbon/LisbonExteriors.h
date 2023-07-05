#pragma once

#include <mutex>

#include "../BuildingStructsActor.h"

class LisbonExteriors {
public:
	LisbonExteriors();
	~LisbonExteriors();

	// Singletons should not be cloneable or assignable.
	LisbonExteriors(LisbonExteriors &other) = delete;
	void operator=(const LisbonExteriors &) = delete;

	static LisbonExteriors *GetInstance();

	static TMap<FString, FPolygon> ParkingLots();

private:
	static LisbonExteriors *pinstance_;
	static std::mutex mutex_;
};
