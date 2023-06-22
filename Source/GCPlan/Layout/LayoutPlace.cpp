#include "LayoutPlace.h"

#include "../DataStructsActor.h"
#include "../Common/Lodash.h"
#include "../Common/MathVector.h"

LayoutPlace::LayoutPlace() {
}

LayoutPlace::~LayoutPlace() {
}

std::tuple<FString, FMeshTransform> LayoutPlace::PlaceMesh(FVector pos, TArray<FString> meshNames,
	FPlaceParams params, FVector pathLine, FVector center) {
	FString meshName = "";
	FString key;
	if (!params.skipMesh) {
		int meshesCount = meshNames.Num();
		int index = Lodash::RandomRangeInt(0, meshesCount - 1);
		meshName = meshNames[index];
		FString uniqueName = Lodash::GetInstanceId(meshName);
		key = uniqueName;
	} else {
		key = Lodash::GetInstanceId();
	}
	FVector rot;
	if (params.rotateTowardCenter) {
		float angle = MathVector::SignedAngle(FVector(1,0,0), pathLine, FVector(0,0,1));
		// Center should be perpendicular to line, so 90 degrees; but need to see if +90 or -90.
		FVector centerAxis = center - pos;
		// See if angle to center is positive or negative.
		float globalAngle = MathVector::SignedAngle(pathLine, centerAxis, FVector(0,0,1));
		int sign = globalAngle < 0 ? -1 : 1;
		angle += sign * 90;
		rot = FVector(0,0,angle);
	} else {
		rot = FVector(Lodash::RandomRangeFloat(params.rotMinX, params.rotMaxX),
			Lodash::RandomRangeFloat(params.rotMinY, params.rotMaxY),
			Lodash::RandomRangeFloat(params.rotMinZ, params.rotMaxZ));
	}
	float scaleFactor = Lodash::RandomRangeFloat(params.scaleMin, params.scaleMax);
	FVector scale = FVector(scaleFactor, scaleFactor, scaleFactor);
	FMeshTransform obj = FMeshTransform(meshName, pos, rot, scale);
	return { key, obj };
}
