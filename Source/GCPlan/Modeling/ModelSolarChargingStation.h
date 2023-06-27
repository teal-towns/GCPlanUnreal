#pragma once

#include "ModelBase.h"

class ModelSolarChargingStation
{
public:
	ModelSolarChargingStation();
	~ModelSolarChargingStation();

	static AActor *Create();
	static void CreateBaseSupport(FString name, FVector scale, FString side, float sizeX, float sizeZ, FActorSpawnParameters spawnParams, FModelParams modelParams, ModelBase *modelBase);
};
