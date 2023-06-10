#include "LayoutPlace.h"

#include "../Common/Lodash.h"
#include "../DataStructsActor.h"

LayoutPlace::LayoutPlace() {
}

LayoutPlace::~LayoutPlace() {
}

std::tuple<FString, FMeshTransform> LayoutPlace::PlaceMesh(FVector pos, TArray<FString> meshNames,
	FPlaceParams params) {
	int meshesCount = meshNames.Num();
	int index = Lodash::RandomRangeInt(0, meshesCount - 1);
	FString meshName = meshNames[index];
	FVector rot = FVector(Lodash::RandomRangeFloat(params.rotMinX, params.rotMaxX),
		Lodash::RandomRangeFloat(params.rotMinY, params.rotMaxY),
		Lodash::RandomRangeFloat(params.rotMinZ, params.rotMaxZ));
	float scaleFactor = Lodash::RandomRangeFloat(params.scaleMin, params.scaleMax);
	FString uniqueName = Lodash::GetInstanceId(meshName);
	FString key = uniqueName;
	FVector scale = FVector(scaleFactor, scaleFactor, scaleFactor);
	FMeshTransform obj = FMeshTransform(meshName, pos, rot, scale);
	return { key, obj };
}
