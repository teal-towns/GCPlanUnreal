#include "PMCorner.h"

#include "Engine/StaticMeshActor.h"
#include "ProceduralMeshComponent.h"

#include "../Common/Lodash.h"
#include "../Common/UnrealGlobal.h"
#include "../Modeling/ModelBase.h"
#include "PMBase.h"

PMCorner::PMCorner() {
}

PMCorner::~PMCorner() {
}

AActor* PMCorner::Create(FVector size, TArray<FString> tags, FModelCreateParams createParams,
	FModelParams modelParams, TArray<FPlaneOffsets> xOffsets, TArray<FPlaneOffsets> yOffsets) {
	UnrealGlobal* unrealGlobal = UnrealGlobal::GetInstance();
	TArray<FVector> Vertices = {};
	TArray<FVector2D> UV0 = {};
	TArray<int> Triangles = {};
	float startRatio = 1;
	float radiusRatio = 1;
	if (xOffsets.Num() < 1) {
		xOffsets = {
			{ FPlaneOffsets(startRatio * 0, 0, 0, -1 * radiusRatio) },
			{ FPlaneOffsets(startRatio * 0.05, 0, 0, -1 * radiusRatio * 0.8) },
			{ FPlaneOffsets(startRatio * 0.1, 0, 0, -1 * radiusRatio * 0.6) },
			{ FPlaneOffsets(startRatio * 0.25, 0, 0, -1 * radiusRatio * 0.4) },
			{ FPlaneOffsets(startRatio * 0.45, 0, 0, -1 * radiusRatio * 0.2) },
			{ FPlaneOffsets(startRatio * 0.75, 0, 0, -1 * radiusRatio * 0.05) },
			{ FPlaneOffsets(startRatio * 1, 0, 0, -1 * radiusRatio * 0) }
		};
	}
	if (yOffsets.Num() < 1) {
		if (tags.Contains("oneSide")) {
			yOffsets = {
				{ FPlaneOffsets(0, 0, 0, 0) },
				{ FPlaneOffsets(1, 0, 0, 0) }
			};
		} else {
			yOffsets = xOffsets;
		}
	}
	// Move up since using negative value for radius (going down).
	FVector offsetCorner = FVector(0,0,size.Z);

	auto [proceduralMesh, meshActor] = PMBase::GetMesh("PMCorner");

	FVector2D verticesCount = FVector2D(xOffsets.Num(), yOffsets.Num());
	int vertexIndex, vertexBottomLeftIndex, vertexBottomRightIndex,
		vertexTopLeftIndex, vertexTopRightIndex;
	float xxVal, xyVal, xzVal, yyVal, yxVal, yzVal, xVal, yVal, zVal;
	// Center x and y, so start negative by half of size.
	float xStart = -1 * size.X / 2;
	float yStart = -1 * size.Y / 2;
	float zStart = 0;
	FVector vertex;
	for (int xx = 0; xx < verticesCount.X; xx++) {
		xxVal = size.X * xOffsets[xx].ratioFromStart;
		xyVal = size.Y * xOffsets[xx].offsetYRatio + xOffsets[xx].offsetY;
		xzVal = size.Z * xOffsets[xx].offsetZRatio + xOffsets[xx].offsetZ;
		for (int yy = 0; yy < verticesCount.Y; yy++) {
			yyVal = size.Y * yOffsets[yy].ratioFromStart;
			yxVal = size.X * yOffsets[yy].offsetXRatio + yOffsets[yy].offsetX;
			yzVal = size.Z * yOffsets[yy].offsetZRatio + yOffsets[yy].offsetZ;
			xVal = xxVal + yxVal;
			yVal = xyVal + yyVal;
			zVal = std::min(xzVal, yzVal);
			vertex = (FVector(xVal, yVal, zVal) + FVector(xStart, yStart, zStart) + offsetCorner + createParams.offset) * unrealGlobal->GetScale();
			Vertices.Add(vertex);

			UV0.Add(FVector2D((float)xx * createParams.UVScale.X, (float)yy * createParams.UVScale.Y));

			// Do 1 quad (6 triangles, 2 vertices) at a time.
			if (yy > 0 && xx < (verticesCount.X - 1)) {
				vertexIndex = (xx * verticesCount.Y) + yy;
				vertexBottomRightIndex = vertexIndex;
				vertexBottomLeftIndex = vertexIndex - 1;
				vertexTopRightIndex = ((xx + 1) * verticesCount.Y) + yy;
				vertexTopLeftIndex = vertexTopRightIndex - 1;

				if (createParams.triangleOrder == "counterClockwiseBottomRight") {
					Triangles.Add(vertexBottomLeftIndex);
					Triangles.Add(vertexBottomRightIndex);
					Triangles.Add(vertexTopLeftIndex);

					Triangles.Add(vertexBottomRightIndex);
					Triangles.Add(vertexTopRightIndex);
					Triangles.Add(vertexTopLeftIndex);
				} else {
					// Need to go with the slope down rather than across it to avoid "steps" instead of smooth surface.
					Triangles.Add(vertexBottomLeftIndex);
					Triangles.Add(vertexTopRightIndex);
					Triangles.Add(vertexTopLeftIndex);

					Triangles.Add(vertexBottomLeftIndex);
					Triangles.Add(vertexBottomRightIndex);
					Triangles.Add(vertexTopRightIndex);
				}
			}
		}
	}

	PMBase::AddMeshSection(proceduralMesh, Vertices, UV0, Triangles, {}, {}, modelParams);
	AStaticMeshActor* actorFinal = PMBase::MeshToActor(Lodash::GetInstanceId("Corner_"), proceduralMesh, createParams, modelParams);
	PMBase::DestroyMesh(meshActor, proceduralMesh);
	return actorFinal;
}
