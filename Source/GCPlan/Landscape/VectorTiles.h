#pragma once

#include <mutex>

#include "../BuildingStructsActor.h"

class VectorTiles {

private:
	static VectorTiles *pinstance_;
	static std::mutex mutex_;

    TArray<FString> _socketKeys = {};
	bool _inited = false;

public:
	VectorTiles();
	~VectorTiles();
	// Singletons should not be cloneable or assignable.
	VectorTiles(VectorTiles &other) = delete;
	void operator=(const VectorTiles &) = delete;

	static VectorTiles *GetInstance();

	void Init();
    void InitSocketOn();
    void Destroy();
	void DestroySocket();
	void GetTiles(float lng, float lat, float xMeters, float yMeters);
};
