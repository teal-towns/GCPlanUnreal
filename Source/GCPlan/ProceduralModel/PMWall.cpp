#include "PMWall.h"

#include "../Common/Lodash.h"
#include "../Common/UnrealGlobal.h"
#include "../Modeling/ModelBase.h"
#include "PMBase.h"
#include "Common/PMTriangle.h"

PMWall::PMWall() {
}

PMWall::~PMWall() {
}

// FVector size = FVector(5,10,1);
// FWall wallParams;
// float z = 0;
// // Door 1 meter from left, 1 meter wide, 3 meters tall.
// float doorLeftY = size.Y / -2 + 1;
// float doorRightY = doorLeftY + 1;
// float doorBottomX = size.X / -2;
// float doorTopX = size.X / -2 + 3;
// // Window 2 meters from right, 1 meter from bottom, 0.5 meters wide, 2 meters tall.
// float windowLeftY = size.Y / 2 - 2;
// float windowRightY = windowLeftY + 0.5;
// float windowBottomX = size.X / -2 + 1;
// float windowTopX = windowBottomX + 2;
// wallParams.holesVertices = {
// 	{
// 		FVector(doorBottomX, doorLeftY, z), FVector(doorTopX, doorLeftY, z),
// 		FVector(doorTopX, doorRightY, z), FVector(doorBottomX, doorRightY, z)
// 	},
// 	{
// 		FVector(windowBottomX, windowLeftY, z), FVector(windowTopX, windowLeftY, z),
// 		FVector(windowTopX, windowRightY, z), FVector(windowBottomX, windowRightY, z)
// 	}
// };
AActor* PMWall::Create(FVector size, FModelCreateParams createParamsIn, FModelParams modelParams,
	FWall params) {
	UnrealGlobal* unrealGlobal = UnrealGlobal::GetInstance();
	auto [proceduralMesh, meshActor] = PMBase::GetMesh("PMWall");
	TArray<FVector> Vertices = {};
	TArray<FVector2D> UV0 = {};
	TArray<int> Triangles = {};
	int vertexBottomLeftIndex, vertexBottomRightIndex,
		vertexTopLeftIndex, vertexTopRightIndex;

	// Go left to right and when get to hole, add vertices for it and above and below it but
	// do not add triangles in middle of it.
	// x = forward (up), y = right (flat plane on ground).
	// Sort holes so they are left to right.
	TArray<FVector> hole;
	for (int ii = 1; ii < params.holesVertices.Num(); ii++) {
		for (int jj = 0; jj < ii; jj++) {
			if (params.holesVertices[ii][0].Y < params.holesVertices[jj][0].Y) {
				hole = params.holesVertices[jj];
				params.holesVertices[jj] = params.holesVertices[ii];
				params.holesVertices[ii] = hole;
				break;
			}
		}
	}

	float currentY = size.Y / -2;
	float minY = size.Y / -2;
	float maxY = size.Y / 2;
	// float minX = size.X / -2;
	// float maxX = size.X / 2;
	float minX = params.minX;
	float maxX = minX + size.X;
	float xRatio;
	float yRatio;
	float prevY = currentY;
	int prevBottomIndex, prevTopIndex, currentBottomIndex, currentTopIndex, prevBottomIndexSave, prevTopIndexSave;
	int holeIndex = 0;
	int holesCount = params.holesVertices.Num();
	float z = holesCount > 0 ? params.holesVertices[0][0].Z : 0;
	int verticesIndex = 0;

	auto AddTopToBottom = [&] (bool addTriangles) {
		yRatio = (currentY - minY) / (maxY - minY);
		// Add top to bottom box to left most vertices.
		Vertices.Add(FVector(minX, currentY, z) * unrealGlobal->GetScale());
		xRatio = 0;
		UV0.Add(FVector2D((float)xRatio * createParamsIn.UVScale.X, (float)yRatio * createParamsIn.UVScale.Y));
		vertexBottomRightIndex = verticesIndex;
		verticesIndex += 1;
		currentBottomIndex = vertexBottomRightIndex;

		Vertices.Add(FVector(maxX, currentY, z) * unrealGlobal->GetScale());
		xRatio = 1;
		UV0.Add(FVector2D((float)xRatio * createParamsIn.UVScale.X, (float)yRatio * createParamsIn.UVScale.Y));
		vertexTopRightIndex = verticesIndex;
		verticesIndex += 1;
		currentTopIndex = vertexTopRightIndex;

		if (currentY > minY && addTriangles) {
			vertexBottomLeftIndex = prevBottomIndex;
			vertexTopLeftIndex = prevTopIndex;
			Triangles = PMTriangle::AddQuad(Triangles, vertexBottomLeftIndex, vertexTopLeftIndex,
				vertexTopRightIndex, vertexBottomRightIndex, params.triangleDirection);
		}
	};

	int iterations = 0;
	int maxIterations = holesCount + 2;
	while (currentY < maxY) {
		// Assumes 4 vertices (rectangle) with order [0] bottom left, [1] top left, [2] top right, [3] bottom right.
		// TODO - handle non rectangles.
		// Add first vertices for left edge.
		if (currentY == minY) {
			// Skip if first hole is on edge.
			bool skip = false;
			if (holeIndex < holesCount && params.holesVertices[holeIndex][0].Y == minY) {
				skip = true;
			}
			if (!skip) {
				AddTopToBottom(true);
				prevBottomIndex = vertexBottomRightIndex;
				prevTopIndex = vertexTopRightIndex;
				// prevY = currentY;
			}
		}

		if (holeIndex < holesCount) {
			// 1. Connect left hole vertices to previous.
			currentY = params.holesVertices[holeIndex][0].Y;
		} else {
			// Connect to end (right most edge).
			currentY = maxY;
		}
		AddTopToBottom(true);
		prevBottomIndex = vertexBottomRightIndex;
		prevTopIndex = vertexTopRightIndex;
		prevY = currentY;

		if (holeIndex < holesCount) {
			hole = params.holesVertices[holeIndex];
			// Add vertices on right side.
			currentY = hole[2].Y;
			AddTopToBottom(false);
			prevBottomIndexSave = vertexBottomRightIndex;
			prevTopIndexSave = vertexTopRightIndex;

			// 2. Add triangles above / below hole.
			// Below
			if (params.holesVertices[holeIndex][0].X > minX) {
				// Add the 2 hole vertices (already have top of wall and bottom of wall ones).
				Vertices.Add(FVector(hole[0].X, hole[0].Y, z) * unrealGlobal->GetScale());
				vertexTopLeftIndex = verticesIndex;
				verticesIndex += 1;
				xRatio = (hole[0].X - minX) / (maxX - minX);
				yRatio = (hole[0].Y - minY) / (maxY - minY);
				UV0.Add(FVector2D((float)xRatio * createParamsIn.UVScale.X, (float)yRatio * createParamsIn.UVScale.Y));

				Vertices.Add(FVector(hole[3].X, hole[3].Y, z) * unrealGlobal->GetScale());
				vertexTopRightIndex = verticesIndex;
				verticesIndex += 1;
				xRatio = (hole[3].X - minX) / (maxX - minX);
				yRatio = (hole[3].Y - minY) / (maxY - minY);
				UV0.Add(FVector2D((float)xRatio * createParamsIn.UVScale.X, (float)yRatio * createParamsIn.UVScale.Y));

				vertexBottomLeftIndex = prevBottomIndex;
				vertexBottomRightIndex = currentBottomIndex;
				Triangles = PMTriangle::AddQuad(Triangles, vertexBottomLeftIndex, vertexTopLeftIndex,
					vertexTopRightIndex, vertexBottomRightIndex, params.triangleDirection);
			}

			// Above
			if (params.holesVertices[holeIndex][1].X < maxX) {
				// Add the 2 hole vertices (already have top of wall and bottom of wall ones).
				Vertices.Add(FVector(hole[1].X, hole[1].Y, z) * unrealGlobal->GetScale());
				vertexBottomLeftIndex = verticesIndex;
				verticesIndex += 1;
				xRatio = (hole[1].X - minX) / (maxX - minX);
				yRatio = (hole[1].Y - minY) / (maxY - minY);
				UV0.Add(FVector2D((float)xRatio * createParamsIn.UVScale.X, (float)yRatio * createParamsIn.UVScale.Y));

				Vertices.Add(FVector(hole[2].X, hole[2].Y, z) * unrealGlobal->GetScale());
				vertexBottomRightIndex = verticesIndex;
				verticesIndex += 1;
				xRatio = (hole[2].X - minX) / (maxX - minX);
				yRatio = (hole[2].Y - minY) / (maxY - minY);
				UV0.Add(FVector2D((float)xRatio * createParamsIn.UVScale.X, (float)yRatio * createParamsIn.UVScale.Y));

				vertexTopLeftIndex = prevTopIndex;
				vertexTopRightIndex = currentTopIndex;
				Triangles = PMTriangle::AddQuad(Triangles, vertexBottomLeftIndex, vertexTopLeftIndex,
					vertexTopRightIndex, vertexBottomRightIndex, params.triangleDirection);
			}

			prevBottomIndex = prevBottomIndexSave;
			prevTopIndex = prevTopIndexSave;
			prevY = currentY;
			holeIndex += 1;
		}

		iterations += 1;
		if (iterations > maxIterations) {
			UE_LOG(LogTemp, Warning, TEXT("PMWall.Create maxIterations break"));
			break;
		}
	}

	PMBase::AddMeshSection(proceduralMesh, Vertices, UV0, Triangles, {}, {}, modelParams);
	AActor* actorFinal = PMBase::MeshToActor(Lodash::GetInstanceId("Wall_"), proceduralMesh, createParamsIn, modelParams);
	PMBase::DestroyMesh(meshActor, proceduralMesh);
	return actorFinal;
}
