#include "PMPlane.h"

#include "Engine/StaticMeshActor.h"
#include "ProceduralMeshComponent.h"

#include "../Common/UnrealGlobal.h"
#include "../Modeling/ModelBase.h"
#include "PMBase.h"

PMPlane::PMPlane() {
}

PMPlane::~PMPlane() {
}

AActor* PMPlane::Create(FString name, FVector size, TArray<FPlaneOffsets> xOffsets,
	TArray<FPlaneOffsets> yOffsets, FString plane, int triangleSide,
	TArray<FString> tags, FModelCreateParams createParams, FModelParams modelParams,
	TMap<FString, FString> offsetsCombine) {
	if (!offsetsCombine.Contains("x")) {
		offsetsCombine.Add("x", "");
	}
	if (!offsetsCombine.Contains("y")) {
		offsetsCombine.Add("y", "");
	}
	if (!offsetsCombine.Contains("z")) {
		offsetsCombine.Add("z", "");
	}

	if (xOffsets.Num() < 1) {
		xOffsets = {
			{ FPlaneOffsets(0) },
			{ FPlaneOffsets(1) }
		};
	}
	if (yOffsets.Num() < 1) {
		yOffsets = {
			{ FPlaneOffsets(0) },
			{ FPlaneOffsets(1) }
		};
	}

	float UVScale = 1;
	UnrealGlobal* unrealGlobal = UnrealGlobal::GetInstance();

	// ModelBase* modelBase = ModelBase::GetInstance();
	// if (!createParams.parentActor) {
	// 	createParams.parentActor = modelBase->CreateActor(name + "_Plane");
	// 	createParams.parent = createParams.parentActor->FindComponentByClass<USceneComponent>();
	// }
	// UObject* parentObject = (UObject*)createParams.parentActor;
	// UProceduralMeshComponent* proceduralMesh = PMBase::CreateMesh(name + "_PlaneMesh", parentObject);
	auto [proceduralMesh, meshActor] = PMBase::GetMesh();

	TArray<FVector> Vertices = {};
	TArray<FVector2D> UV0 = {};
	TArray<int> Triangles = {};

	// Draw plane in X (forward) and Y (right) with Z up.
	FVector2D verticesCount = FVector2D(xOffsets.Num(), yOffsets.Num());
	int vertexIndex, vertexBottomLeftIndex, vertexBottomRightIndex,
		vertexTopLeftIndex, vertexTopRightIndex;
	float xxVal, xyVal, xzVal, yyVal, yxVal, yzVal, xVal, yVal, zVal;
	// Center x and y, so start negative by half of size.
	float xStart = -1 * size.X / 2;
	float yStart = -1 * size.Y / 2;
	float zStart = 0;
	if (plane == "xz") {
		yStart = -1 * size.Z / 2;
		zStart = 0;
	} else if (plane == "yz") {
		xStart = -1 * size.Z / 2;
		zStart = 0;
	}
	FVector vertexMapped;
	for (int xx = 0; xx < verticesCount.X; xx++) {
		xxVal = size.X * xOffsets[xx].ratioFromStart;
		xyVal = size.Y * xOffsets[xx].offsetYRatio + xOffsets[xx].offsetY;
		xzVal = size.Z * xOffsets[xx].offsetZRatio + xOffsets[xx].offsetZ;
		for (int yy = 0; yy < verticesCount.Y; yy++) {
			yyVal = size.Y * yOffsets[yy].ratioFromStart;
			yxVal = size.X * yOffsets[yy].offsetXRatio + yOffsets[yy].offsetX;
			yzVal = size.Z * yOffsets[yy].offsetZRatio + yOffsets[yy].offsetZ;
			// Combine x and y instructions for offset.
			if (offsetsCombine["x"] == "min") {
				xVal = std::min(xxVal, yxVal);
			} else if (offsetsCombine["x"] == "max") {
				xVal = std::max(xxVal, yxVal);
			} else {
				xVal = xxVal + yxVal;
			}
			if (offsetsCombine["y"] == "min") {
				yVal = std::min(xyVal, yyVal);
			} else if (offsetsCombine["y"] == "max") {
				yVal = std::max(xyVal, yyVal);
			} else {
				yVal = xyVal + yyVal;
			}
			if (offsetsCombine["z"] == "min") {
				zVal = std::min(xzVal, yzVal);
			} else if (offsetsCombine["z"] == "max") {
				zVal = std::max(xzVal, yzVal);
			} else {
				zVal = xzVal + yzVal;
			}
			// UE_LOG(LogTemp, Display, TEXT("%d %d %f %f %f xxVal %f yxVal %f"), xx, yy, xVal, yVal, zVal, xxVal, yxVal);

			vertexMapped = MapXYZ(FVector(xVal, yVal, zVal), plane);
			vertexMapped += FVector(xStart, yStart, zStart) + createParams.offset;
			vertexMapped *= unrealGlobal->GetScale();
			Vertices.Add(vertexMapped);
			// UE_LOG(LogTemp, Display, TEXT("vertexMapped %s"), *vertexMapped.ToString());

			UV0.Add(FVector2D((float)xx * UVScale, (float)yy * UVScale));

			// Do 1 quad (6 triangles, 2 vertices) at a time.
			if (yy > 0 && xx < (verticesCount.X - 1)) {
				vertexIndex = (xx * verticesCount.Y) + yy;
				vertexBottomRightIndex = vertexIndex;
				vertexBottomLeftIndex = vertexIndex - 1;
				vertexTopRightIndex = ((xx + 1) * verticesCount.Y) + yy;
				vertexTopLeftIndex = vertexTopRightIndex - 1;
				if (triangleSide > 0) {
					Triangles.Add(vertexBottomLeftIndex);
					Triangles.Add(vertexBottomRightIndex);
					Triangles.Add(vertexTopLeftIndex);
					Triangles.Add(vertexBottomRightIndex);
					Triangles.Add(vertexTopRightIndex);
					Triangles.Add(vertexTopLeftIndex);
				} else {
					// Reverse order (otherwise inside is visible and outside is not).
					Triangles.Add(vertexBottomLeftIndex);
					Triangles.Add(vertexTopLeftIndex);
					Triangles.Add(vertexBottomRightIndex);
					Triangles.Add(vertexBottomRightIndex);
					Triangles.Add(vertexTopLeftIndex);
					Triangles.Add(vertexTopRightIndex);
				}
				// UE_LOG(LogTemp, Display, TEXT("Triangles %d %d %d %d %d %d"), vertexBottomLeftIndex, vertexBottomRightIndex, vertexTopLeftIndex, vertexBottomRightIndex, vertexTopRightIndex, vertexTopLeftIndex);
			}
		}
	}

	FString modelP = modelParams.parent ? modelParams.parent->GetName() : "null";
	FString createP = createParams.parent ? createParams.parent->GetName() : "null";
	PMBase::AddMeshSection(proceduralMesh, Vertices, UV0, Triangles, {}, {}, modelParams);
	AStaticMeshActor* actorFinal = PMBase::MeshToActor(name + "_Plane", proceduralMesh, createParams, modelParams);
	PMBase::DestroyMesh(meshActor, proceduralMesh);
	return actorFinal;
	// return proceduralMesh;
}

FVector PMPlane::MapXYZ(FVector xyz, FString plane) {
	FVector vertexMapped = xyz;
	if (plane == "xz") {
		vertexMapped = FVector(vertexMapped.X, vertexMapped.Z , vertexMapped.Y);
	} else if (plane == "yz") {
		vertexMapped = FVector(vertexMapped.Z, vertexMapped.Y, vertexMapped.X);
	}
	return vertexMapped;
}

// std::tuple<TArray<FPlaneOffsets>, TArray<FPlaneOffsets>> PMPlane::RoundedOffsets() {
// 	TArray<FPlaneOffsets> xOffsets = {
// 		{ FPlaneOffsets(0, 0, 0, 0, 0, 0, -0.5) },
// 		{ FPlaneOffsets(0.02, 0, 0, 0, 0, 0, -0.4) },
// 		{ FPlaneOffsets(0.04, 0, 0, 0, 0, 0, -0.3) },
// 		{ FPlaneOffsets(0.08, 0, 0, 0, 0, 0, -0.2) },
// 		{ FPlaneOffsets(0.16, 0, 0, 0, 0, 0, -0.1) },
// 		{ FPlaneOffsets(0.32, 0, 0, 0, 0, 0, -0.05) },
// 		{ FPlaneOffsets(1 - 0.32, 0, 0, 0, 0, 0, -0.05) },
// 		{ FPlaneOffsets(1 - 0.16, 0, 0, 0, 0, 0, -0.1) },
// 		{ FPlaneOffsets(1 - 0.08, 0, 0, 0, 0, 0, -0.2) },
// 		{ FPlaneOffsets(1 - 0.04, 0, 0, 0, 0, 0, -0.3) },
// 		{ FPlaneOffsets(1 - 0.02, 0, 0, 0, 0, 0, -0.4) },
// 		{ FPlaneOffsets(1, 0, 0, 0, 0, 0, -0.5) }
// 	};
// 	TArray<FPlaneOffsets> yOffsets = {
// 		// { FPlaneOffsets(0, 0, 0, 0, 0, 0, -0.5) },
// 		// { FPlaneOffsets(0.02, 0, 0, 0, 0, 0, -0.4) },
// 		// { FPlaneOffsets(0.04, 0, 0, 0, 0, 0, -0.3) },
// 		// { FPlaneOffsets(0.08, 0, 0, 0, 0, 0, -0.2) },
// 		// { FPlaneOffsets(0.16, 0, 0, 0, 0, 0, -0.1) },
// 		// { FPlaneOffsets(0.32, 0, 0, 0, 0, 0, -0.05) },
// 		// { FPlaneOffsets(1 - 0.32, 0, 0, 0, 0, 0, -0.05) },
// 		// { FPlaneOffsets(1 - 0.16, 0, 0, 0, 0, 0, -0.1) },
// 		// { FPlaneOffsets(1 - 0.08, 0, 0, 0, 0, 0, -0.2) },
// 		// { FPlaneOffsets(1 - 0.04, 0, 0, 0, 0, 0, -0.3) },
// 		// { FPlaneOffsets(1 - 0.02, 0, 0, 0, 0, 0, -0.4) },
// 		// { FPlaneOffsets(1, 0, 0, 0, 0, 0, -0.5) }
// 		{ FPlaneOffsets(0) },
// 		{ FPlaneOffsets(1) }
// 	};
// 	return { xOffsets, yOffsets };
// }

// TArray<FPlaneOffsets> PMPlane::BorderRadiusTop(float radiusRatio, float startRatio,
// 	FString anchor) {
// 	TArray<FPlaneOffsets> offsets = {};
// 	if (anchor == "start") {
// 		offsets = {
// 			{ FPlaneOffsets(startRatio * 0, 0, 0, 0, 0, 0, -1 * radiusRatio) },
// 			{ FPlaneOffsets(startRatio * 0.05, 0, 0, 0, 0, 0, -1 * radiusRatio * 0.8) },
// 			{ FPlaneOffsets(startRatio * 0.1, 0, 0, 0, 0, 0, -1 * radiusRatio * 0.6) },
// 			{ FPlaneOffsets(startRatio * 0.25, 0, 0, 0, 0, 0, -1 * radiusRatio * 0.4) },
// 			{ FPlaneOffsets(startRatio * 0.45, 0, 0, 0, 0, 0, -1 * radiusRatio * 0.2) },
// 			{ FPlaneOffsets(startRatio * 0.75, 0, 0, 0, 0, 0, -1 * radiusRatio * 0.1) }
// 		};
// 	} else {
// 		offsets = {
// 			{ FPlaneOffsets(1 - startRatio * 0.75, 0, 0, 0, 0, 0, -1 * radiusRatio * 0.1) },
// 			{ FPlaneOffsets(1 - startRatio * 0.45, 0, 0, 0, 0, 0, -1 * radiusRatio * 0.2) },
// 			{ FPlaneOffsets(1 - startRatio * 0.25, 0, 0, 0, 0, 0, -1 * radiusRatio * 0.4) },
// 			{ FPlaneOffsets(1 - startRatio * 0.1, 0, 0, 0, 0, 0, -1 * radiusRatio * 0.6) },
// 			{ FPlaneOffsets(1 - startRatio * 0.05, 0, 0, 0, 0, 0, -1 * radiusRatio * 0.8) },
// 			{ FPlaneOffsets(1 - startRatio * 0, 0, 0, 0, 0, 0, -1 * radiusRatio) }
// 		};
// 	}
// 	return offsets;
// }
