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

bool LayoutPolygon::PlaceInPolygon(TArray<FVector> vertices, TArray<FString> meshNames, 
	FPlaceParams inParams, FVector posCenter) {
	if (meshNames.Num() < 1) {
		return false;
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
			inParams.skipPolygons2D.Add(MathPolygon::PointsTo2D(inParams.skipPolygons[ii]));
		}
	}

	if (inParams.maxCount < 0 && inParams.maxCountMin >= 0 && inParams.maxCountMax > 0) {
		inParams.maxCount = Lodash::RandomRangeInt(inParams.maxCountMin, inParams.maxCountMax);
	}

	TArray<FVector2D> vertices2D = {};
	FVector min = FVector(posCenter.X - radius, posCenter.Y - radius, posCenter.Z + minZOffset);
	FVector max = FVector(posCenter.X + radius, posCenter.Y + radius, posCenter.Z + maxZOffset);
	if (vertices.Num() > 0) {
		min = FVector(0,0,0);
		max = FVector(0,0,0);
		min.Z = posCenter.Z + minZOffset;
		max.Z = posCenter.Z + maxZOffset;
		for (int ii = 0; ii < vertices.Num(); ii++) {
			if (min.X == 0 || vertices[ii].X < min.X) {
				min.X = vertices[ii].X;
			}
			if (max.X == 0 || vertices[ii].X > max.X) {
				max.X = vertices[ii].X;
			}
			if (min.Y == 0 || vertices[ii].Y < min.Y) {
				min.Y = vertices[ii].Y;
			}
			if (max.Y == 0 || vertices[ii].Y > max.Y) {
				max.Y = vertices[ii].Y;
			}
		}

		vertices2D = MathPolygon::PointsTo2D(vertices);
	}

	TMap<FString, FMeshTransform> meshes = {};

	int count = 0;
	float curX = min.X;
	float curY = min.Y;
	float offsetMaxX = offsetAverage * offsetMaxFactorX;
	float offsetMaxY = offsetAverage * offsetMaxFactorY;
	float offsetX, offsetY;
	bool done = false;

	FVector pos;
	int iterations = 0;

	if (inParams.spreadPattern == "random") {
		if (inParams.maxCount < 0) {
			inParams.maxCount = 5;
		}
		while (count < inParams.maxCount) {
			pos = FVector(Lodash::RandomRangeFloat(min.X, max.X),
				Lodash::RandomRangeFloat(min.Y, max.Y),
				Lodash::RandomRangeFloat(min.Z, max.Z));
			auto [key, obj] = CheckAddObj(pos, meshNames, vertices2D, posCenter, inParams);
			if (key.Len() > 0) {
				meshes.Add(key, obj);
				count += 1;
			}
		}
	} else {
		while (curY < max.Y) {
			curX = min.X;
			while (curX < max.X) {
				offsetX = Lodash::RandomRangeFloat(-1 * offsetMaxX, offsetMaxX);
				offsetY = Lodash::RandomRangeFloat(-1 * offsetMaxY, offsetMaxY);
				pos = FVector(curX + offsetX, curY + offsetY, posCenter.Z);

				auto [key, obj] = CheckAddObj(pos, meshNames, vertices2D, posCenter, inParams);
				if (key.Len() > 0) {
					meshes.Add(key, obj);
					count += 1;
					if (inParams.maxCount > 0 && count > inParams.maxCount) {
						done = true;
						break;
					}
				}
				iterations += 1;

				curX += offsetAverage;
			}
			curY += offsetAverage;
			if (done) {
				break;
			}
		}
	}

	// Place them.
	InstancedMesh* instancedMesh = InstancedMesh::GetInstance();
	for (auto& Elem : meshes) {
		instancedMesh->CreateInstance(Elem.Value.meshName, Elem.Value.location,
			DataConvert::VectorToRotator(Elem.Value.rotation), Elem.Value.scale);
	}
	// return meshes;
	return true;
}

std::tuple<FString, FMeshTransform> LayoutPolygon::CheckAddObj(FVector pos,
	TArray<FString> meshNames, TArray<FVector2D> vertices2D, FVector posCenter,
	FPlaceParams inParams) {
	FString key = "";
	FMeshTransform obj = FMeshTransform();

	FString meshName;
	FString name, uniqueName;
	FVector scale, rot;
	// TArray<FString> emptyTArray = {};
	FVector piece;
	int index;
	float scaleFactor;
	int meshesCount = meshNames.Num();
	FVector2D pos2D = FVector2D(pos.X, pos.Y);

	HeightMap* heightMap = HeightMap::GetInstance();
	// See if inside vertices / shape.
	if (InPolygon(pos, vertices2D, posCenter, inParams.radius, inParams.shape)) {
		bool valid = true;

		if (inParams.skipPolygons2D.Num() > 0) {
			for (int ii = 0; ii < inParams.skipPolygons2D.Num(); ii++) {
				if (MathPolygon::IsPointInPolygon(pos2D, inParams.skipPolygons2D[ii])) {
					valid = false;
					break;
				}
			}
		}

		if (inParams.snapToGround) {
			pos.Z = heightMap->GetTerrainHeightAtPoint(FVector(pos.X, pos.Y, 0));
		}
		if (valid) {
			index = Lodash::RandomRangeInt(0, meshesCount - 1);
			meshName = meshNames[index];
			rot = FVector(Lodash::RandomRangeFloat(inParams.rotMinX, inParams.rotMaxX),
				Lodash::RandomRangeFloat(inParams.rotMinY, inParams.rotMaxY),
				Lodash::RandomRangeFloat(inParams.rotMinZ, inParams.rotMaxZ));
			scaleFactor = Lodash::RandomRangeFloat(inParams.scaleMin, inParams.scaleMax);
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

bool LayoutPolygon::InPolygon(FVector pos, TArray<FVector2D> vertices2D, FVector posCenter,
	float radius, FString shape) {
	if (vertices2D.Num() > 0) {
		FVector2D pos2D = FVector2D(pos.X, pos.Y);
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
