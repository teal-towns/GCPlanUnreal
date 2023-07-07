#pragma once

#include <mutex>

#include "../BuildingStructsActor.h"

class LisbonInteriors {
public:
	LisbonInteriors();
	~LisbonInteriors();

	// Singletons should not be cloneable or assignable.
	LisbonInteriors(LisbonInteriors &other) = delete;
	void operator=(const LisbonInteriors &) = delete;

	static LisbonInteriors *GetInstance();

	static TMap<FString, FPolygon> Rooms(float zOffset = 0);

private:
	static LisbonInteriors *pinstance_;
	static std::mutex mutex_;
};
