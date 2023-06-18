#pragma once

#include "Engine/StaticMeshActor.h"

class ModelCouch {
public:
	ModelCouch();
	~ModelCouch();

	static AStaticMeshActor* Create();
};
