#pragma once

#include <mutex>
#include "ProceduralMeshComponent.h"

#include "../BuildingStructsActor.h"

class MeshTerrain {

private:
	static MeshTerrain *pinstance_;
	static std::mutex mutex_;

	// static UWorld* _world;

	UProceduralMeshComponent* ProceduralMesh;
	TArray<FVector> Vertices;
	TArray<FVector2D> UV0;
	TArray<int> Triangles;
	UMaterialInterface* Material;
	UMaterialInstanceDynamic* MaterialInstanceDynamic;
	// UMaterialInstance* MaterialInstance;
	TMap<FString, TMap<FString, FPolygon>> _RoadsByType = {};

public:
	MeshTerrain();
	~MeshTerrain();

	// Singletons should not be cloneable or assignable.
	MeshTerrain(MeshTerrain &other) = delete;
	void operator=(const MeshTerrain &) = delete;

	static MeshTerrain *GetInstance();

	void SetProceduralMesh(UProceduralMeshComponent*, UMaterialInterface*);
	// void SetProceduralMesh(UProceduralMeshComponent*, UMaterialInstance*);
	void ClearMesh();

	void CreateTerrainFromHeightMap(float Scale = 247.71, int UVScale = 1);
	std::tuple<int, int> CreateVerticesFromHeightMap(float Scale, int UVScale, float ZScale = 100);
	void CreateTriangles(int XSize, int YSize);
	void AddRoads(TMap<FString, FPolygon> roads);
	void DrawRoads(float stepFactor = 0.5);
	void DestroyRoads();
};
