#include "PMPlanePolygon.h"

#include "Engine/StaticMeshActor.h"
#include "ProceduralMeshComponent.h"

#include "../Common/Lodash.h"
#include "../Common/MathPolygon.h"
#include "../Common/UnrealGlobal.h"
#include "../Modeling/ModelBase.h"
#include "PMBase.h"

PMPlanePolygon::PMPlanePolygon() {
}

PMPlanePolygon::~PMPlanePolygon() {
}

AActor* PMPlanePolygon::Create(TArray<FVector> vertices, FModelCreateParams createParams,
	FModelParams modelParams, FPlanePolygon params) {
	UnrealGlobal* unrealGlobal = UnrealGlobal::GetInstance();
	auto [proceduralMesh, meshActor] = PMBase::GetMesh("PMPlanePolygon");

	TArray<FVector> Vertices = {};
	TArray<FVector2D> UV0 = {};
	TArray<int> Triangles = {};

	TArray<FVector> bounds = MathPolygon::Bounds(vertices);
	FVector min = bounds[0];
	FVector max = bounds[1];

	FVector path, nextVertex, vertex, midPoint, triangleStart, point;
	int nextVertexIndex;
	float xRatio, yRatio;
	// Will draw triangles from single point if possible, but if cross line out of polygon, will move it.
	int triangleStartIndex = 0;
	bool doTriangles;
	TArray<FVector> points;
	// bool looping, atLeastOneIntersection;
	// float distance, angle;
	for (int ii = 0; ii < vertices.Num(); ii++) {
		vertex = vertices[ii];
		nextVertexIndex = ii < vertices.Num() - 1 ? ii + 1 : 0;
		nextVertex = vertices[nextVertexIndex];

		Vertices.Add(vertices[ii] * unrealGlobal->GetScale());
		xRatio = (vertices[ii].X - min.X) / (max.X - min.X);
		yRatio = (vertices[ii].Y - min.Y) / (max.Y - min.Y);
		UV0.Add(FVector2D((float)xRatio * createParams.UVScale.X, (float)yRatio * createParams.UVScale.Y));
		if (ii >= 2) {
			doTriangles = true;
			// TODO - not working properly.
			// // See if either line crosses outside of polygon.
			// looping = triangleStartIndex < ii - 1 - 1 ? true : false;
			// if (nextVertexIndex == triangleStartIndex) {
			// 	looping = false;
			// }
			// while (looping) {
			// 	triangleStart = vertices[triangleStartIndex];
			// 	points = { vertices[ii-1], vertices[ii] };
			// 	atLeastOneIntersection = false;
			// 	for (int pp = 0; pp < points.Num(); pp++) {
			// 		point = points[pp];
			// 		// Check all lines.
			// 		for (int ix = 0; ix < ii; ix++) {
			// 			int pointIndex = pp == 0 ? ii - 1 : ii;
			// 			UE_LOG(LogTemp, Display, TEXT("triangleStart %d pointIndex %d ix %d ix + 1 %d"), triangleStartIndex, pointIndex, ix, ix + 1);
			// 			auto [intersects, xIntersect, yIntersect] = MathPolygon::GetLineIntersection(
			// 				triangleStart.X, triangleStart.Y, point.X, point.Y,
			// 				vertices[ix].X, vertices[ix].Y,
			// 				vertices[ix + 1].X, vertices[ix + 1].Y);
			// 			if (intersects) {
			// 				atLeastOneIntersection = true;
			// 				UE_LOG(LogTemp, Display, TEXT("intersects ii %d ix %d triangleStartIndex %d"), ii, ix, triangleStartIndex);
			// 				break;
			// 			}
			// 		}
			// 		if (atLeastOneIntersection) {
			// 			break;
			// 		}
			// 	}
			// 	if (atLeastOneIntersection) {
			// 		triangleStartIndex += 1;
			// 		if (triangleStartIndex >= ii - 1 - 1) {
			// 			doTriangles = false;
			// 			looping = false;
			// 		}
			// 		if (nextVertexIndex == triangleStartIndex) {
			// 			looping = false;
			// 		}
			// 	} else {
			// 		looping = false;
			// 	}
			// }

			if (doTriangles) {
				if (params.triangleDirection == "clockwise") {
					Triangles.Add(triangleStartIndex);
					Triangles.Add(ii - 1);
					Triangles.Add(ii);
				} else {
					Triangles.Add(triangleStartIndex);
					Triangles.Add(ii);
					Triangles.Add(ii - 1);
				}
			}
		}
	}

	PMBase::AddMeshSection(proceduralMesh, Vertices, UV0, Triangles, {}, {}, modelParams);
	AStaticMeshActor* actorFinal = PMBase::MeshToActor(Lodash::GetInstanceId(params.name + "PlanePolygon_"), proceduralMesh, createParams, modelParams);
	PMBase::DestroyMesh(meshActor, proceduralMesh);
	return actorFinal;
}
