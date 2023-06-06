#pragma once

#include <mutex>
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"
#include "ProceduralMeshComponent.h"
#include "Engine/StaticMeshActor.h"

#include "../BuildingStructsActor.h"

class SplineRoad {

private:
	static SplineRoad *pinstance_;
	static std::mutex mutex_;

	UWorld* World;

	TMap<FString, TMap<FString, FRoadPath>> _RoadsByType = {};
	static AStaticMeshActor* _roadsActor;
	TMap<FString, AActor*> _RoadsActors = {};

	USplineMeshComponent* InitMesh(FString UName, UObject* parentObject, USceneComponent* parent, float widthMeters);
	void AddSplineMesh(FString UName, int pointCount, UObject* parentObject, USceneComponent* parent, float widthMeters,
		USplineComponent* spline);

public:
	SplineRoad();
	~SplineRoad();

	// Singletons should not be cloneable or assignable.
	SplineRoad(SplineRoad &other) = delete;
	void operator=(const SplineRoad &) = delete;

	static SplineRoad *GetInstance();
	void SetWorld(UWorld*);

	void AddRoads(TMap<FString, FRoadPath> roads);
	void DrawRoads();
	void DestroyRoads();
};
