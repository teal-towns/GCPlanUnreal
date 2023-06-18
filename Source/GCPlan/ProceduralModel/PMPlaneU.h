#pragma once

#include "PMBase.h"
#include "../Modeling/ModelBase.h"

class PMPlaneU {
public:
	PMPlaneU();
	~PMPlaneU();

	static AActor* Create(FString name, FVector size,
		FModelCreateParams createParams = FModelCreateParams(), FModelParams modelParams = FModelParams());
};
