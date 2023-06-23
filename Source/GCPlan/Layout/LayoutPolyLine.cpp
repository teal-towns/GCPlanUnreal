#include "LayoutPolyLine.h"

#include "LayoutPlace.h"
#include "../DataStructsActor.h"
#include "../Common/DataConvert.h"
#include "../Common/Lodash.h"
#include "../Common/MathPolygon.h"
#include "../Common/MathVector.h"
#include "../Landscape/HeightMap.h"
#include "../Mesh/InstancedMesh.h"

LayoutPolyLine* LayoutPolyLine::pinstance_{nullptr};
std::mutex LayoutPolyLine::mutex_;

LayoutPolyLine::LayoutPolyLine() {
}

LayoutPolyLine::~LayoutPolyLine() {
}

LayoutPolyLine *LayoutPolyLine::GetInstance() {
	std::lock_guard<std::mutex> lock(mutex_);
	if (pinstance_ == nullptr) {
		pinstance_ = new LayoutPolyLine();
	}
	return pinstance_;
}

TMap<FString, FMeshTransform> LayoutPolyLine::PlaceOnLine(TArray<FVector> vertices, TArray<FString> meshNames,
	FPlaceParams params) {
	TMap<FString, FMeshTransform> meshes = {};
	if (!params.skipMesh && meshNames.Num() < 1) {
		return meshes;
	}

	HeightMap* heightMap = HeightMap::GetInstance();
	FVector center = MathPolygon::GetPolygonCenter(vertices);

	FVector pathLine, vertex, vertexNext, currentPoint, crossAxisLine, crossPoint, crossPointEnd, vertexEnd;
	int indexNext;
	float stepDistance, currentAngle, currentAngleCross, lineDistance, spacingActual;
	int verticesCount = vertices.Num();
	float crossDegrees = 90;
	float widthRadius = params.width / 2;
	int iterations, iterationsCross;
	int maxIterations, maxIterationsCross, itemCount;
	int buffer = 5;
	for (int vv = 0; vv < verticesCount; vv++) {
		if (vv == verticesCount - 1 && params.closedLoop < 1) {
			break;
		}
		vertex = vertices[vv];
		indexNext = (vv < verticesCount - 1) ? vv + 1 : 0;
		vertexNext = vertices[indexNext];
		pathLine = vertexNext - vertex;
		currentPoint = vertices[vv] + pathLine.GetClampedToMaxSize(params.spacingStart);
		vertexEnd = vertexNext - pathLine.GetClampedToMaxSize(params.spacingEnd);
		lineDistance = (vertexEnd - currentPoint).Size();
		itemCount = floor(lineDistance / (params.spacing - params.spacingFactor));
		if (itemCount < params.minPerEdge) {
			itemCount = params.minPerEdge;
		}
		if (params.alignCenter > 0) {
			spacingActual = (float)(lineDistance / (itemCount + 1));
			currentPoint += pathLine.GetClampedToMaxSize(spacingActual);
		}

		currentAngle = MathVector::SignedAngle(pathLine, (vertexEnd - currentPoint), FVector(0,0,1));
		iterations = 0;
		maxIterations = ceil(pathLine.Size() / (params.spacing - params.spacingFactor)) + buffer;
		while (abs(currentAngle) < 30) {
			if (params.spacingCrossAxis < params.width) {
				// Go along cross axis for width.
				crossAxisLine = pathLine.RotateAngleAxis(crossDegrees, FVector(0,0,1));
				crossPointEnd = currentPoint + crossAxisLine.GetClampedToMaxSize(widthRadius);
				// Start is backwards along line.
				crossPoint = currentPoint - crossAxisLine.GetClampedToMaxSize(widthRadius);
				stepDistance = params.spacingCrossAxis + Lodash::RandomRangeFloat(0,
					params.spacingCrossAxis * params.spacingFactorCrossAxis);
				crossPoint += crossAxisLine.GetClampedToMaxSize(stepDistance);
				// To know when done (past end point), find angle between cross axis and the vector from
				// current point to end point. This should be (near) 0 degrees while in front of line, and negative when behind.
				currentAngleCross = MathVector::SignedAngle(crossAxisLine, (crossPointEnd - crossPoint), FVector(0,0,1));
				iterationsCross = 0;
				maxIterationsCross = ceil(params.width / (params.spacingCrossAxis - params.spacingFactorCrossAxis)) + buffer;
				while (abs(currentAngleCross) < 30) {
					auto [key1, obj1] = LayoutPlace::PlaceMesh(crossPoint, meshNames, params, pathLine, center);
					meshes.Add(key1, obj1);

					stepDistance = params.spacingCrossAxis + Lodash::RandomRangeFloat(-1 * params.spacingCrossAxis * params.spacingFactorCrossAxis,
						params.spacingCrossAxis * params.spacingFactorCrossAxis);
					crossPoint += crossAxisLine.GetClampedToMaxSize(stepDistance);
					if (params.snapToGround) {
						crossPoint.Z = heightMap->GetTerrainHeightAtPoint(FVector(crossPoint.X, crossPoint.Y, 0));
					}
					currentAngleCross = MathVector::SignedAngle(crossAxisLine, (crossPointEnd - crossPoint), FVector(0,0,1));
					iterationsCross += 1;
					if (iterationsCross > maxIterationsCross) {
						UE_LOG(LogTemp, Warning, TEXT("LayoutPolyLine.PlaceOnLine max iterationsCross break %d"), vv);
					}
				}
			} else {
				// If did cross, cross line should have already placed on / near center point, otherwise place in center.
				auto [key1, obj1] = LayoutPlace::PlaceMesh(currentPoint, meshNames, params, pathLine, center);
				meshes.Add(key1, obj1);
			}

			// Find next point on main axis.
			stepDistance = params.spacing + Lodash::RandomRangeFloat(-1 * params.spacing * params.spacingFactor,
				params.spacing * params.spacingFactor);
			currentPoint += pathLine.GetClampedToMaxSize(stepDistance);
			if (params.snapToGround) {
				currentPoint.Z = heightMap->GetTerrainHeightAtPoint(FVector(currentPoint.X, currentPoint.Y, 0));
			}
			currentAngle = MathVector::SignedAngle(pathLine, (vertexEnd - currentPoint), FVector(0,0,1));
			iterations += 1;
			if (iterations > maxIterations) {
				UE_LOG(LogTemp, Warning, TEXT("LayoutPolyLine.PlaceOnLine max iterations break %d"), vv);
			}
		}
	}

	// Place them.
	if (!params.skipMesh) {
		InstancedMesh* instancedMesh = InstancedMesh::GetInstance();
		for (auto& Elem : meshes) {
			instancedMesh->CreateInstance(Elem.Value.meshName, Elem.Value.location,
				DataConvert::VectorToRotator(Elem.Value.rotation), Elem.Value.scale);
		}
	}

	return meshes;
}

bool LayoutPolyLine::PlaceOnLineSides(TArray<FVector> vertices, float width, TArray<FString> meshNames,
	FPlaceParams params) {
	TArray<FVector> verticesOuter, verticesInner;
	if (vertices.Num() > 2) {
		// UE_LOG(LogTemp, Display, TEXT("width %f"), width);
		FVector center = MathPolygon::GetPolygonCenter(vertices);
		verticesOuter = MathPolygon::BufferVertices(vertices, center, width / 2);
		verticesInner = MathPolygon::BufferVertices(vertices, center, -1 * width / 2); 
	} else {
		FVector pathLine = vertices[1] - vertices[0];
		FVector crossAxisClamped = pathLine.RotateAngleAxis(90, FVector(0,0,1)).GetClampedToMaxSize(width / 2);
		verticesOuter = { vertices[0] + crossAxisClamped, vertices[1] + crossAxisClamped };
		verticesInner = { vertices[0] - crossAxisClamped, vertices[1] - crossAxisClamped };
	}
	PlaceOnLine(verticesOuter, meshNames, params);
	PlaceOnLine(verticesInner, meshNames, params);
	return true;
}
