#include "PMPlaneU.h"

#include "Engine/StaticMeshActor.h"
#include "ProceduralMeshComponent.h"

#include "../Common/Lodash.h"
#include "../Common/UnrealGlobal.h"
#include "../Modeling/ModelBase.h"
#include "PMBase.h"
#include "PMPlane.h"
#include "PMSpline.h"

PMPlaneU::PMPlaneU() {
}

PMPlaneU::~PMPlaneU() {
}

AActor* PMPlaneU::Create(FString name, FVector size, FModelCreateParams createParams, FModelParams modelParams,
	FString direction, TArray<FPlaneOffsets> xOffsets, TArray<FPlaneOffsets> yOffsets) {
	if (xOffsets.Num() < 1) {
		xOffsets = {
			{ FPlaneOffsets(0, 0, 0, 0) },
			{ FPlaneOffsets(1, 0, 0, 0) }
		};
	}
	if (yOffsets.Num() < 1) {
		yOffsets = GetOffsetsStart(direction);
		yOffsets += GetOffsetsEnd(direction);
	}
	AActor* actor = PMPlane::Create(name + "_U", size, xOffsets, yOffsets, "xy", createParams, modelParams);
	return actor;
}

TArray<FPlaneOffsets> PMPlaneU::GetOffsetsStart(FString direction, float ratioFactor) {
	TArray<FString> offsetRatioKeys = { direction };
	TArray<FPlaneOffsets> offsets = {
		{ FPlaneOffsets(0 * ratioFactor, 0, offsetRatioKeys) },
		{ FPlaneOffsets(0.02 * ratioFactor, 0.5, offsetRatioKeys) },
		{ FPlaneOffsets(0.04 * ratioFactor, 0.6, offsetRatioKeys) },
		{ FPlaneOffsets(0.08 * ratioFactor, 0.7, offsetRatioKeys) },
		{ FPlaneOffsets(0.16 * ratioFactor, 0.8, offsetRatioKeys) },
		{ FPlaneOffsets(0.24 * ratioFactor, 0.9, offsetRatioKeys) },
		{ FPlaneOffsets(0.32 * ratioFactor, 0.95, offsetRatioKeys) },
		{ FPlaneOffsets(0.4 * ratioFactor, 1, offsetRatioKeys) },
	};
	return offsets;
}

TArray<FPlaneOffsets> PMPlaneU::GetOffsetsEnd(FString direction, float ratioFactor) {
	TArray<FString> offsetRatioKeys = { direction };
	TArray<FPlaneOffsets> offsets = {
		{ FPlaneOffsets(1 - 0.4 * ratioFactor, 1, offsetRatioKeys) },
		{ FPlaneOffsets(1 - 0.32 * ratioFactor, 0.95, offsetRatioKeys) },
		{ FPlaneOffsets(1 - 0.24 * ratioFactor, 0.9, offsetRatioKeys) },
		{ FPlaneOffsets(1 - 0.16 * ratioFactor, 0.8, offsetRatioKeys) },
		{ FPlaneOffsets(1 - 0.08 * ratioFactor, 0.7, offsetRatioKeys) },
		{ FPlaneOffsets(1 - 0.04 * ratioFactor, 0.6, offsetRatioKeys) },
		{ FPlaneOffsets(1 - 0.02 * ratioFactor, 0.5, offsetRatioKeys) },
		{ FPlaneOffsets(1 - 0 * ratioFactor, 0, offsetRatioKeys) }
	};
	return offsets;
}

// U with thickness (2 sides with a top and bottom to connect them).
AActor* PMPlaneU::Shape(FString name, FVector size, FModelCreateParams createParams, FModelParams modelParams,
	float thickness) {
	FVector offsetParent = createParams.offset;
	createParams.offset = FVector(0,0,0);
	TArray<FPlaneOffsets> xOffsets, yOffsets;
	// Go up (curve) in Z direction.
	// xOffsets = GetOffsetsStart("z", 0.25);
	// xOffsets += GetOffsetsEnd("z", 0.25);
	// xOffsets = {
	// 	{ FPlaneOffsets(0, 0, 0, 0, 0, 0, -0.5 * thickness) },
	// 	{ FPlaneOffsets(0.01, 0, 0, 0) },
	// 	{ FPlaneOffsets(1 - 0.01, 0, 0, 0) },
	// 	{ FPlaneOffsets(1, 0, 0, 0, 0, 0, -0.5 * thickness) }
	// };

	ModelBase* modelBase = ModelBase::GetInstance();
	AActor* actor = modelBase->CreateActor(name + Lodash::GetInstanceId("PlaneUShape"));
	USceneComponent* parent = actor->FindComponentByClass<USceneComponent>();
	FModelParams groupModelParams = modelParams;
	groupModelParams.parent = parent;
	FModelCreateParams groupCreateParams;
	groupCreateParams.parentActor = actor;
	groupCreateParams.parent = parent;

	AActor* actorTemp;
	Create(name + "Outer", size, groupCreateParams, groupModelParams);
	// Create another one thickness away, with opposite triangles.
	createParams.offset = FVector(0,0, -1 * thickness);
	FVector scale = FVector(size.X, size.Y - thickness * 2, size.Z - thickness);
	createParams.triangleSide = -1;
	Create(name + "Inner", scale, groupCreateParams, groupModelParams);
	createParams.offset = FVector(0,0,0);

	// Create attachment to connect on top and bottom.
	// Create simple 10x10 plane as spline mesh.
	xOffsets = {};
	yOffsets = {};
	int count = 10;
	for (int ii = 0; ii <= count; ii++) {
		xOffsets.Add(FPlaneOffsets(ii / count, 0, 0, 0));
		yOffsets.Add(FPlaneOffsets(ii / count, 0, 0, 0));
	}
	actorTemp = PMPlane::Create("Plane", FVector(thickness * 3, thickness, 0.01), xOffsets, yOffsets,
		"xy", groupCreateParams, groupModelParams);
	UStaticMeshComponent* meshComponent = actorTemp->FindComponentByClass<UStaticMeshComponent>();
	modelParams.mesh = meshComponent->GetStaticMesh();
	// Create in YZ plane to match orientation of rest of shape.
	yOffsets = GetOffsetsStart("z");
	yOffsets += GetOffsetsEnd("z");
	// In between outer and inner.
	scale = FVector(size.X, size.Y - thickness, size.Z - thickness / 2);
	TArray<FVector> vertices = PMPlane::VerticesFromOffsets(scale, yOffsets);
	AActor* actorSpline = PMSpline::Create(name, vertices, thickness, groupCreateParams, groupModelParams);

	xOffsets = {
		{ FPlaneOffsets(0, 0, 0, 0) },
		{ FPlaneOffsets(1, 0, 0, 0) }
	};
	groupCreateParams.triangleSide = 0;
	groupCreateParams.offsetsCombine = { { "x", "min" } };
	scale = FVector(size.Z, size.Y, size.X);
	actorTemp = Create(name + "Seat",  scale, groupCreateParams, groupModelParams, "x", xOffsets);
	ModelBase::SetTransform(actorTemp, FVector(size.Z / 2, 0, size.X), FVector(0,90,0));

	return actor;
}
