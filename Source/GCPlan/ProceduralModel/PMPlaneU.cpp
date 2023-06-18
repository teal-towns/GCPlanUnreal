#include "PMPlaneU.h"

#include "Engine/StaticMeshActor.h"
#include "ProceduralMeshComponent.h"

#include "../Common/UnrealGlobal.h"
#include "../Modeling/ModelBase.h"
#include "PMPlane.h"
#include "PMBase.h"

PMPlaneU::PMPlaneU() {
}

PMPlaneU::~PMPlaneU() {
}

AActor* PMPlaneU::Create(FString name, FVector size, FModelCreateParams createParams, FModelParams modelParams) {
	TArray<FPlaneOffsets> xOffsets = {
		{ FPlaneOffsets(0) },
		{ FPlaneOffsets(1) }
	};
	TArray<FPlaneOffsets> yOffsets = {
		{ FPlaneOffsets(0, 0, 0, 0) },
		{ FPlaneOffsets(0.02, 0, 0, 0.5) },
		{ FPlaneOffsets(0.04, 0, 0, 0.6) },
		{ FPlaneOffsets(0.08, 0, 0, 0.7) },
		{ FPlaneOffsets(0.16, 0, 0, 0.8) },
		{ FPlaneOffsets(0.24, 0, 0, 0.9) },
		{ FPlaneOffsets(0.32, 0, 0, 0.95) },
		{ FPlaneOffsets(0.4, 0, 0, 1) },
		{ FPlaneOffsets(1 - 0.4, 0, 0, 1) },
		{ FPlaneOffsets(1 - 0.32, 0, 0, 0.95) },
		{ FPlaneOffsets(1 - 0.24, 0, 0, 0.9) },
		{ FPlaneOffsets(1 - 0.16, 0, 0, 0.8) },
		{ FPlaneOffsets(1 - 0.08, 0, 0, 0.7) },
		{ FPlaneOffsets(1 - 0.04, 0, 0, 0.6) },
		{ FPlaneOffsets(1 - 0.02, 0, 0, 0.5) },
		{ FPlaneOffsets(1, 0, 0, 0) }
	};
	AActor* actor = PMPlane::Create(name + "_U", size, xOffsets, yOffsets, "xy", 1, {}, createParams, modelParams);
	return actor;
}

// U with thickness (2 sides with a top and bottom to connect them).
// AActor* PMPlaneU::Shape(FString name, FVector size, FModelCreateParams createParams, FModelParams modelParams) {

// }
