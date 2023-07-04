#include "LayoutPolygon.h"

#include "LayoutPlace.h"
#include "../DataStructsActor.h"
#include "../Common/DataConvert.h"
#include "../Common/Lodash.h"
#include "../Common/MathPolygon.h"
#include "../Common/UnrealGlobal.h"
#include "../Landscape/HeightMap.h"
#include "../Mesh/InstancedMesh.h"

LayoutPolygon* LayoutPolygon::pinstance_{nullptr};
std::mutex LayoutPolygon::mutex_;

LayoutPolygon::LayoutPolygon() {
}

LayoutPolygon::~LayoutPolygon() {
}

LayoutPolygon *LayoutPolygon::GetInstance() {
	std::lock_guard<std::mutex> lock(mutex_);
	if (pinstance_ == nullptr) {
		pinstance_ = new LayoutPolygon();
	}
	return pinstance_;
}

TMap<FString, FMeshTransform> LayoutPolygon::PlaceInPolygon(TArray<FVector> vertices, TArray<FString> meshNames, 
	FPlaceParams inParams, FVector posCenter) {
	TMap<FString, FMeshTransform> meshes = {};
	if (!inParams.skipMesh && meshNames.Num() < 1) {
		return meshes;
	}
	if (posCenter == FVector(0,0,0)) {
		posCenter = MathPolygon::GetPolygonCenter(vertices);
	}
	bool snapToGround = inParams.snapToGround;
	float radius = inParams.radius;
	FString shape = inParams.shape;
	float offsetAverage = inParams.offsetAverage;
	float offsetMaxFactorX = inParams.offsetMaxFactorX;
	float offsetMaxFactorY = inParams.offsetMaxFactorY;
	float offsetMaxFactorZ = inParams.offsetMaxFactorZ;
	float maxZOffset = inParams.maxZOffset;
	float minZOffset = inParams.minZOffset;

	// Form 2d version (just once for performance).
	if (inParams.skipPolygons.Num() > 0 && inParams.skipPolygons2D.Num() < 1) {
		for (int ii = 0; ii < inParams.skipPolygons.Num(); ii++) {
			inParams.skipPolygons2D.Add(MathPolygon::PointsTo2D(inParams.skipPolygons[ii], inParams.plane));
		}
	}

	if (inParams.maxCount < 0 && inParams.maxCountMin >= 0 && inParams.maxCountMax > 0) {
		inParams.maxCount = Lodash::RandomRangeInt(inParams.maxCountMin, inParams.maxCountMax);
	}

	TArray<FVector2D> vertices2D = {};
	FVector min = FVector(posCenter.X - radius, posCenter.Y - radius, posCenter.Z + minZOffset);
	FVector max = FVector(posCenter.X + radius, posCenter.Y + radius, posCenter.Z + maxZOffset);
	if (inParams.plane == "xz") {
		min = FVector(posCenter.X - radius, posCenter.Y, posCenter.Z - radius);
		max = FVector(posCenter.X + radius, posCenter.Y, posCenter.Z + radius);
	} else if (inParams.plane == "yz") {
		min = FVector(posCenter.X, posCenter.Y - radius, posCenter.Z - radius);
		max = FVector(posCenter.X, posCenter.Y + radius, posCenter.Z + radius);
	}
	if (vertices.Num() > 0) {
		vertices2D = MathPolygon::PointsTo2D(vertices, inParams.plane);
		TArray<FVector> bounds = MathPolygon::Bounds(vertices);
		min = bounds[0];
		max = bounds[1];
		if (inParams.plane == "xy") {
			min.Z = posCenter.Z + minZOffset;
			max.Z = posCenter.Z + maxZOffset;
		}
		// min = FVector(0,0,0);
		// max = FVector(0,0,0);
		// min.Z = posCenter.Z + minZOffset;
		// max.Z = posCenter.Z + maxZOffset;
		// for (int ii = 0; ii < vertices.Num(); ii++) {
		// 	if (min.X == 0 || vertices[ii].X < min.X) {
		// 		min.X = vertices[ii].X;
		// 	}
		// 	if (max.X == 0 || vertices[ii].X > max.X) {
		// 		max.X = vertices[ii].X;
		// 	}
		// 	if (min.Y == 0 || vertices[ii].Y < min.Y) {
		// 		min.Y = vertices[ii].Y;
		// 	}
		// 	if (max.Y == 0 || vertices[ii].Y > max.Y) {
		// 		max.Y = vertices[ii].Y;
		// 	}
		// }
	}

	int count = 0;
	FVector pos;
	int iterations = 0;

	// Get a line to approximate the plane this is on. This cannot just be min and max,
	// it needs to be actual points (not just bounds, otherwise would always be the same
	// path / plane instead of matching the input vertices).
	FString axis = "x";
	if (inParams.plane == "yz") {
		axis = "y";
	}
	TArray<FVector> minMaxPoints = MathPolygon::MinMaxPoints(vertices, axis);

	if (inParams.spreadPattern == "random") {
		if (inParams.maxCount < 0) {
			inParams.maxCount = 5;
		}
		while (count < inParams.maxCount) {
			pos = FVector(Lodash::RandomRangeFloat(min.X, max.X),
				Lodash::RandomRangeFloat(min.Y, max.Y),
				Lodash::RandomRangeFloat(min.Z, max.Z));
			auto [key, obj] = CheckAddObj(pos, meshNames, vertices2D, posCenter, minMaxPoints, inParams);
			if (key.Len() > 0) {
				meshes.Add(key, obj);
				count += 1;
			}
		}
	} else {
		float curX = min.X;
		float curY = min.Y;
		float offsetMaxX = offsetAverage * offsetMaxFactorX;
		float offsetMaxY = offsetAverage * offsetMaxFactorY;
		float offsetX, offsetY;
		bool done = false;
		// Use x & y, but map as needed.
		float minX = min.X;
		float minY = min.Y;
		float maxX = max.X;
		float maxY = max.Y;
		if (inParams.plane == "xz") {
			curY = min.Z;
			minY = min.Z;
			maxY = max.Z;
			offsetMaxY = offsetAverage * offsetMaxFactorZ;
		} else if (inParams.plane == "yz") {
			curX = min.Z;
			minX = min.Z;
			maxX = max.Z;
			offsetMaxX = offsetAverage * offsetMaxFactorZ;
		}
		float offsetXBase = inParams.offsetX > 0 ? inParams.offsetX : offsetAverage;
		float offsetYBase = inParams.offsetY > 0 ? inParams.offsetY : offsetAverage;
		while (curY < maxY) {
			curX = minX;
			while (curX < maxX) {
				offsetX = Lodash::RandomRangeFloat(-1 * offsetMaxX, offsetMaxX);
				offsetY = Lodash::RandomRangeFloat(-1 * offsetMaxY, offsetMaxY);
				if (inParams.plane == "xz") {
					pos = FVector(curX + offsetX, posCenter.Y, curY + offsetY);
				} else if (inParams.plane == "yz") {
					pos = FVector(posCenter.X, curY + offsetY, curX + offsetX);
				} else {
					pos = FVector(curX + offsetX, curY + offsetY, posCenter.Z);
				}

				auto [key, obj] = CheckAddObj(pos, meshNames, vertices2D, posCenter, minMaxPoints, inParams);
				if (key.Len() > 0) {
					meshes.Add(key, obj);
					count += 1;
					if (inParams.maxCount > 0 && count > inParams.maxCount) {
						done = true;
						break;
					}
				}
				iterations += 1;

				curX += offsetXBase;
			}
			curY += offsetYBase;
			if (done) {
				break;
			}
		}
	}

	// Place them.
	if (!inParams.skipMesh) {
		InstancedMesh* instancedMesh = InstancedMesh::GetInstance();
		for (auto& Elem : meshes) {
			instancedMesh->CreateInstance(Elem.Value.meshName, Elem.Value.location,
				DataConvert::VectorToRotator(Elem.Value.rotation), Elem.Value.scale);
		}
	}
	return meshes;
}

std::tuple<FString, FMeshTransform> LayoutPolygon::CheckAddObj(FVector pos,
	TArray<FString> meshNames, TArray<FVector2D> vertices2D, FVector posCenter,
	TArray<FVector> minMaxPoints, FPlaceParams inParams) {
	FString key = "";
	FMeshTransform obj = FMeshTransform();

	FString meshName = "";
	FString name, uniqueName;
	FVector scale, rot;
	// TArray<FString> emptyTArray = {};
	FVector piece;
	int index;
	float scaleFactor;
	int meshesCount = meshNames.Num();
	FVector2D pos2D = FVector2D(pos.X, pos.Y);
	if (inParams.plane == "xz") {
		pos2D = FVector2D(pos.X, pos.Z);
	} else if (inParams.plane == "yz") {
		pos2D = FVector2D(pos.Y, pos.Z);
	}

	HeightMap* heightMap = HeightMap::GetInstance();
	// See if inside vertices / shape.
	if (InPolygon(pos, pos2D, vertices2D, posCenter, inParams.radius, inParams.shape)) {
		bool valid = true;

		if (inParams.skipPolygons2D.Num() > 0) {
			for (int ii = 0; ii < inParams.skipPolygons2D.Num(); ii++) {
				if (MathPolygon::IsPointInPolygon(pos2D, inParams.skipPolygons2D[ii])) {
					valid = false;
					break;
				}
			}
		}

		if (inParams.snapToGround && inParams.plane == "xy") {
			pos.Z = heightMap->GetTerrainHeightAtPoint(FVector(pos.X, pos.Y, 0));
		} else {
			// Use projection (assumes straight line from max to min points) to get 3rd coordinate).
			// https://math.stackexchange.com/questions/404440/what-is-the-equation-for-a-3d-line
			FVector min = minMaxPoints[0];
			FVector max = minMaxPoints[1];
			float t;
			if (inParams.plane == "xy") {
				t = (pos.X - min.X) / (max.X - min.X);
				pos.Z = min.Z + t * (max.Z - min.Z);
			} else if (inParams.plane == "xz") {
				t = (pos.X - min.X) / (max.X - min.X);
				pos.Y = min.Y + t * (max.Y - min.Y);
			} else if (inParams.plane == "yz") {
				t = (pos.Y - min.Y) / (max.Y - min.Y);
				pos.X = min.X + t * (max.X - min.X);
			}
		}
		if (valid) {
			rot = FVector(Lodash::RandomRangeFloat(inParams.rotMinX, inParams.rotMaxX),
				Lodash::RandomRangeFloat(inParams.rotMinY, inParams.rotMaxY),
				Lodash::RandomRangeFloat(inParams.rotMinZ, inParams.rotMaxZ));
			scaleFactor = Lodash::RandomRangeFloat(inParams.scaleMin, inParams.scaleMax);
			if (meshesCount > 0) {
				index = Lodash::RandomRangeInt(0, meshesCount - 1);
				meshName = meshNames[index];
			}
			uniqueName = Lodash::GetInstanceId(meshName);
			key = uniqueName;
			scale = FVector(scaleFactor, scaleFactor, scaleFactor);
			obj = FMeshTransform(meshName, pos, rot, scale);
		}
	// } else {
	// 	UE_LOG(LogTemp, Display, TEXT("NOT in polygon %s"), *pos.ToString());
	}

	return { key, obj };
}

bool LayoutPolygon::InPolygon(FVector pos, FVector2D pos2D, TArray<FVector2D> vertices2D, FVector posCenter,
	float radius, FString shape) {
	if (vertices2D.Num() > 0) {
		if (MathPolygon::IsPointInPolygon(pos2D, vertices2D)) {
			return true;
		} else {
			return false;
		}
	}
	if (shape == "circle") {
		// Calculate distance to center and see if bigger than radius.
		float posRadius = (pos - posCenter).Size();
		if (posRadius > radius) {
			return false;
		} else {
			return true;
		}
	}
	return true;
}
