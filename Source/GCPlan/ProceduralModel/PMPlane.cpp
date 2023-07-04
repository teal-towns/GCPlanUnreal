#include "PMPlane.h"

#include "Engine/StaticMeshActor.h"
#include "ProceduralMeshComponent.h"

#include "../Common/Lodash.h"
#include "../Common/UnrealGlobal.h"
#include "../Modeling/ModelBase.h"
#include "PMBase.h"

PMPlane::PMPlane() {
}

PMPlane::~PMPlane() {
}

AActor* PMPlane::Create(FString name, FVector size, TArray<FPlaneOffsets> xOffsets,
	TArray<FPlaneOffsets> yOffsets, FString plane,
	FModelCreateParams createParams, FModelParams modelParams) {
	TMap<FString, FString> offsetsCombine = createParams.offsetsCombine;
	// auto [xOffsets1, yOffsets1, offsetsCombine1] = GetInputs(xOffsets, yOffsets, offsetsCombine);
	// xOffsets = xOffsets1;
	// yOffsets = yOffsets1;
	// offsetsCombine = offsetsCombine1;
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
			{ FPlaneOffsets(0, 0, 0, 0) },
			{ FPlaneOffsets(1, 0, 0, 0) }
		};
	}
	if (yOffsets.Num() < 1) {
		yOffsets = {
			{ FPlaneOffsets(0, 0, 0, 0) },
			{ FPlaneOffsets(1, 0, 0, 0) }
		};
	}

	UnrealGlobal* unrealGlobal = UnrealGlobal::GetInstance();

	auto [proceduralMesh, meshActor] = PMBase::GetMesh("PMPlane");

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
	FVector vertexMapped, vertexTopLeft, diff;
	FString steeperDiagonal;
	// float xDistance, yDistance;
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

			// vertexMapped = ComputeVertex(xx, yy, size, xOffsets, yOffsets, plane,
			// 	createParams, offsetsCombine, xStart, yStart, zStart);

			Vertices.Add(vertexMapped * unrealGlobal->GetScale());

			UV0.Add(FVector2D((float)xOffsets[xx].ratioFromStart * createParams.UVScale.X, (float)yOffsets[yy].ratioFromStart * createParams.UVScale.Y));

			if (yy > 0 && xx > 0) {
				vertexIndex = (xx * verticesCount.Y) + yy;
				vertexTopRightIndex = vertexIndex;
				vertexTopLeftIndex = vertexIndex - 1;
				vertexBottomRightIndex = ((xx - 1) * verticesCount.Y) + yy;
				vertexBottomLeftIndex = vertexBottomRightIndex - 1;
				// Go along the slope to get smooth triangles.
				steeperDiagonal = "bottomRight";
				if (abs(Vertices[vertexBottomLeftIndex].Z - Vertices[vertexTopRightIndex].Z) >
					abs(Vertices[vertexBottomRightIndex].Z - Vertices[vertexTopLeftIndex].Z)) {
					steeperDiagonal = "bottomLeft";
				}

				if (createParams.triangleSide >= 0) {
					// counter clockwise
					// Will draw diagonal from bottom right to top left OR bottom left to top right.
					if (steeperDiagonal == "bottomRight") {
						Triangles.Add(vertexBottomLeftIndex);
						Triangles.Add(vertexBottomRightIndex);
						Triangles.Add(vertexTopLeftIndex);
						Triangles.Add(vertexBottomRightIndex);
						Triangles.Add(vertexTopRightIndex);
						Triangles.Add(vertexTopLeftIndex);
					} else {
						Triangles.Add(vertexBottomLeftIndex);
						Triangles.Add(vertexBottomRightIndex);
						Triangles.Add(vertexTopRightIndex);
						Triangles.Add(vertexBottomLeftIndex);
						Triangles.Add(vertexTopRightIndex);
						Triangles.Add(vertexTopLeftIndex);
					}
				}
				if (createParams.triangleSide <= 0) {
					// clockwise
					// Reverse order (otherwise inside is visible and outside is not).
					if (steeperDiagonal == "bottomRight") {
						Triangles.Add(vertexBottomLeftIndex);
						Triangles.Add(vertexTopLeftIndex);
						Triangles.Add(vertexBottomRightIndex);
						Triangles.Add(vertexBottomRightIndex);
						Triangles.Add(vertexTopLeftIndex);
						Triangles.Add(vertexTopRightIndex);
					} else {
						Triangles.Add(vertexBottomLeftIndex);
						Triangles.Add(vertexTopLeftIndex);
						Triangles.Add(vertexTopRightIndex);
						Triangles.Add(vertexBottomRightIndex);
						Triangles.Add(vertexBottomLeftIndex);
						Triangles.Add(vertexTopRightIndex);
					}
				}

			// 	// Find smallest difference; the triangles will be along the other 2 axes.
			// 	diff = (Vertices[vertexTopRightIndex] - Vertices[vertexBottomLeftIndex]) / unrealGlobal->GetScale();
			// 	// default to Z being smallest.
			// 	float UVx = xOffsets[xx].ratioFromStart;
			// 	float UVy = yOffsets[yy].ratioFromStart;
			// 	FString smallest = "z";
			// 	float diffX = abs(diff.X);
			// 	float diffY = abs(diff.Y);
			// 	float diffZ = abs(diff.Z);
			// 	if (diffX < diffY && diffX < diffZ) {
			// 		// X is smallest, so along the YZ plane (use Z as X).
			// 		// UVx = diffZ / size.X;
			// 		UVx = ((Vertices[vertexIndex] / unrealGlobal->GetScale()).Z - zStart) / size.X;
			// 		smallest = "x";
			// 	} else if (diffY < diffX && diffY < diffZ) {
			// 		// Y is smallest, so along the XZ plane (use Z as Y).
			// 		// UVy = diffZ / size.Y;
			// 		UVx = ((Vertices[vertexIndex] / unrealGlobal->GetScale()).Z - zStart) / size.Y;
			// 		smallest = "y";
			// 	}
			// 	UE_LOG(LogTemp, Display, TEXT("xx %d yy %d diff %s UV x %f y %f xRatio %f yRatio %f vertex %s smallest %s"), xx, yy, *diff.ToString(), UVx, UVy, xOffsets[xx].ratioFromStart, yOffsets[yy].ratioFromStart, *vertexMapped.ToString(), *smallest);
			// 	UV0.Add(FVector2D((float)UVx * createParams.UVScale.X, (float)UVy * createParams.UVScale.Y));
			// } else {
			// 	// TODO - compute (x) for this case too?
			// 	// UV0.Add(FVector2D((float)xx * createParams.UVScale.X, (float)yy * createParams.UVScale.Y));
			// 	UE_LOG(LogTemp, Display, TEXT("xx %d yy %d UV x %f y %f"), xx, yy, xOffsets[xx].ratioFromStart, yOffsets[yy].ratioFromStart);
			// 	UV0.Add(FVector2D((float)xOffsets[xx].ratioFromStart * createParams.UVScale.X, (float)yOffsets[yy].ratioFromStart * createParams.UVScale.Y));
			}

			// // Do 1 quad (6 triangles, 2 vertices) at a time.
			// if (yy > 0 && xx < (verticesCount.X - 1)) {
			// 	vertexIndex = (xx * verticesCount.Y) + yy;
			// 	vertexBottomRightIndex = vertexIndex;
			// 	vertexBottomLeftIndex = vertexIndex - 1;
			// 	vertexTopRightIndex = ((xx + 1) * verticesCount.Y) + yy;
			// 	vertexTopLeftIndex = vertexTopRightIndex - 1;
			// 	if (createParams.triangleSide > 0) {
			// 		Triangles.Add(vertexBottomLeftIndex);
			// 		Triangles.Add(vertexBottomRightIndex);
			// 		Triangles.Add(vertexTopLeftIndex);
			// 		Triangles.Add(vertexBottomRightIndex);
			// 		Triangles.Add(vertexTopRightIndex);
			// 		Triangles.Add(vertexTopLeftIndex);
			// 	} else {
			// 		// Reverse order (otherwise inside is visible and outside is not).
			// 		Triangles.Add(vertexBottomLeftIndex);
			// 		Triangles.Add(vertexTopLeftIndex);
			// 		Triangles.Add(vertexBottomRightIndex);
			// 		Triangles.Add(vertexBottomRightIndex);
			// 		Triangles.Add(vertexTopLeftIndex);
			// 		Triangles.Add(vertexTopRightIndex);
			// 	}
			// 	// UE_LOG(LogTemp, Display, TEXT("Triangles %d %d %d %d %d %d"), vertexBottomLeftIndex, vertexBottomRightIndex, vertexTopLeftIndex, vertexBottomRightIndex, vertexTopRightIndex, vertexTopLeftIndex);

			// 	// xDistance = (Vertices[vertexBottomRightIndex] - Vertices[vertexBottomLeftIndex]).Size();
			// 	// // Do not have top vertex yet, so compute it.
			// 	// // TODO - avoid doing this twice for performance.
			// 	// vertexTopLeft = ComputeVertex((xx + 1), yy, size, xOffsets, yOffsets, plane,
			// 	// 	createParams, offsetsCombine, xStart, yStart, zStart);
			// 	// yDistance = (vertexTopLeft - Vertices[vertexBottomLeftIndex]).Size();
			// 	// UV0.Add(FVector2D((float)xDistance * createParams.UVScale.X, (float)yDistance * createParams.UVScale.Y));
			// // } else {
			// // 	// TODO calculate this (x) too? Y should always be 0 the first time so is correct?
			// // 	// UV0.Add(FVector2D((float)xx * createParams.UVScale.X, (float)yy * createParams.UVScale.Y));
			// // 	UV0.Add(FVector2D((float)xOffsets[xx].ratioFromStart * createParams.UVScale.X, (float)yOffsets[yy].ratioFromStart * createParams.UVScale.Y));
			// }
		}
	}

	PMBase::AddMeshSection(proceduralMesh, Vertices, UV0, Triangles, {}, {}, modelParams);
	AStaticMeshActor* actorFinal = PMBase::MeshToActor(name + Lodash::GetInstanceId("_Plane"), proceduralMesh, createParams, modelParams);
	PMBase::DestroyMesh(meshActor, proceduralMesh);
	return actorFinal;
}

// std::tuple<TArray<FPlaneOffsets>, TArray<FPlaneOffsets>, TMap<FString, FString>> PMPlane::GetInputs(
// 	TArray<FPlaneOffsets> xOffsets, TArray<FPlaneOffsets> yOffsets,
// 	TMap<FString, FString> offsetsCombine) {
// 	if (!offsetsCombine.Contains("x")) {
// 		offsetsCombine.Add("x", "");
// 	}
// 	if (!offsetsCombine.Contains("y")) {
// 		offsetsCombine.Add("y", "");
// 	}
// 	if (!offsetsCombine.Contains("z")) {
// 		offsetsCombine.Add("z", "");
// 	}

// 	if (xOffsets.Num() < 1) {
// 		xOffsets = {
// 			{ FPlaneOffsets(0, 0, 0, 0) },
// 			{ FPlaneOffsets(1, 0, 0, 0) }
// 		};
// 	}
// 	if (yOffsets.Num() < 1) {
// 		yOffsets = {
// 			{ FPlaneOffsets(0, 0, 0, 0) },
// 			{ FPlaneOffsets(1, 0, 0, 0) }
// 		};
// 	}
// 	return { xOffsets, yOffsets, offsetsCombine };
// }

TArray<FVector> PMPlane::VerticesFromOffsets(FVector size, TArray<FPlaneOffsets> offsets, FVector offset) {
	TArray<FVector> vertices = {};
	float xVal, yVal, zVal;
	FVector start = FVector(-1 * size.X / 2, -1 * size.Y / 2, 0);
	FVector vertex;
	for (int ii = 0; ii < offsets.Num(); ii++) {
		xVal = size.X * offsets[ii].ratioFromStart;
		yVal = size.Y * offsets[ii].offsetYRatio + offsets[ii].offsetY;
		zVal = size.Z * offsets[ii].offsetZRatio + offsets[ii].offsetZ;
		vertex = FVector(xVal, yVal, zVal) + start + offset;
		vertices.Add(vertex);
		UE_LOG(LogTemp, Display, TEXT("ii %d vertex %s"), ii, *vertex.ToString());
	}
	return vertices;
}

// TArray<FVector> PMPlane::ComputeVertices(FString name, FVector size, TArray<FPlaneOffsets> xOffsets,
// 	TArray<FPlaneOffsets> yOffsets, FString plane,
// 	TArray<FString> tags, FModelCreateParams createParams, FModelParams modelParams) {
// 	TMap<FString, FString> offsetsCombine = createParams.offsetsCombine;
// 	auto [xOffsets1, yOffsets1, offsetsCombine1] = GetInputs(xOffsets, yOffsets, offsetsCombine);
// 	xOffsets = xOffsets1;
// 	yOffsets = yOffsets1;
// 	offsetsCombine = offsetsCombine1;

// 	UnrealGlobal* unrealGlobal = UnrealGlobal::GetInstance();
// 	TArray<FVector> vertices = {};

// 	// Draw plane in X (forward) and Y (right) with Z up.
// 	FVector2D verticesCount = FVector2D(xOffsets.Num(), yOffsets.Num());
// 	float xxVal, xyVal, xzVal, yyVal, yxVal, yzVal, xVal, yVal, zVal;
// 	// Center x and y, so start negative by half of size.
// 	float xStart = -1 * size.X / 2;
// 	float yStart = -1 * size.Y / 2;
// 	float zStart = 0;
// 	if (plane == "xz") {
// 		yStart = -1 * size.Z / 2;
// 		zStart = 0;
// 	} else if (plane == "yz") {
// 		xStart = -1 * size.Z / 2;
// 		zStart = 0;
// 	}
// 	FVector vertexMapped;
// 	for (int xx = 0; xx < verticesCount.X; xx++) {
// 		xxVal = size.X * xOffsets[xx].ratioFromStart;
// 		xyVal = size.Y * xOffsets[xx].offsetYRatio + xOffsets[xx].offsetY;
// 		xzVal = size.Z * xOffsets[xx].offsetZRatio + xOffsets[xx].offsetZ;
// 		for (int yy = 0; yy < verticesCount.Y; yy++) {
// 			yyVal = size.Y * yOffsets[yy].ratioFromStart;
// 			yxVal = size.X * yOffsets[yy].offsetXRatio + yOffsets[yy].offsetX;
// 			yzVal = size.Z * yOffsets[yy].offsetZRatio + yOffsets[yy].offsetZ;
// 			// Combine x and y instructions for offset.
// 			if (offsetsCombine["x"] == "min") {
// 				xVal = std::min(xxVal, yxVal);
// 			} else if (offsetsCombine["x"] == "max") {
// 				xVal = std::max(xxVal, yxVal);
// 			} else {
// 				xVal = xxVal + yxVal;
// 			}
// 			if (offsetsCombine["y"] == "min") {
// 				yVal = std::min(xyVal, yyVal);
// 			} else if (offsetsCombine["y"] == "max") {
// 				yVal = std::max(xyVal, yyVal);
// 			} else {
// 				yVal = xyVal + yyVal;
// 			}
// 			if (offsetsCombine["z"] == "min") {
// 				zVal = std::min(xzVal, yzVal);
// 			} else if (offsetsCombine["z"] == "max") {
// 				zVal = std::max(xzVal, yzVal);
// 			} else {
// 				zVal = xzVal + yzVal;
// 			}
// 			vertexMapped = MapXYZ(FVector(xVal, yVal, zVal), plane);
// 			vertexMapped += FVector(xStart, yStart, zStart) + createParams.offset;
// 			vertices.Add(vertexMapped * unrealGlobal->GetScale());
// 		}
// 	}
// 	return vertices;
// }

FVector PMPlane::ComputeVertex(int xx, int yy, FVector size, TArray<FPlaneOffsets> xOffsets,
	TArray<FPlaneOffsets> yOffsets, FString plane, FModelCreateParams createParams,
	float xStart, float yStart, float zStart) {
	TMap<FString, FString> offsetsCombine = createParams.offsetsCombine;
	float xxVal = size.X * xOffsets[xx].ratioFromStart;
	float xyVal = size.Y * xOffsets[xx].offsetYRatio + xOffsets[xx].offsetY;
	float xzVal = size.Z * xOffsets[xx].offsetZRatio + xOffsets[xx].offsetZ;
	float yyVal = size.Y * yOffsets[yy].ratioFromStart;
	float yxVal = size.X * yOffsets[yy].offsetXRatio + yOffsets[yy].offsetX;
	float yzVal = size.Z * yOffsets[yy].offsetZRatio + yOffsets[yy].offsetZ;
	float xVal, yVal, zVal;
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

	FVector vertexMapped = MapXYZ(FVector(xVal, yVal, zVal), plane);
	vertexMapped += FVector(xStart, yStart, zStart) + createParams.offset;
	UnrealGlobal* unrealGlobal = UnrealGlobal::GetInstance();
	vertexMapped *= unrealGlobal->GetScale();
	return vertexMapped;
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
