#pragma once

#include "../Modeling/ModelBase.h"

class BuildingRoom {
public:
	BuildingRoom();
	~BuildingRoom();

	static void Build(TMap<FString, FString> pairs);
	static void Wall(FVector size, FModelParams modelParams = FModelParams(),
		FModelCreateParams createParamsIn = FModelCreateParams(),
		TMap<FString, FString> pairs = {});
	static void Cube(FVector size, FModelParams modelParams = FModelParams(),
		FModelCreateParams createParamsIn = FModelCreateParams(),
		TMap<FString, FString> pairs = {});
};
