#include "SplineRoad.h"

#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"
#include "Engine/StaticMeshActor.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "ProceduralMeshComponent.h"

#include "../Common/DataConvert.h"
#include "../Common/Lodash.h"
#include "../Common/UnrealGlobal.h"
#include "../Landscape/HeightMap.h"
#include "../Layout/LayoutPlace.h"
#include "../Layout/LayoutPolyLine.h"
#include "../Mesh/InstancedMesh.h"
#include "../Mesh/LoadContent.h"
#include "../Modeling/ModelBase.h"
#include "../ProceduralModel/PMBase.h"
#include "../ProceduralModel/PMSpline.h"

SplineRoad* SplineRoad::pinstance_{nullptr};
std::mutex SplineRoad::mutex_;

SplineRoad::SplineRoad() {
}

SplineRoad::~SplineRoad() {
}

SplineRoad *SplineRoad::GetInstance() {
	std::lock_guard<std::mutex> lock(mutex_);
	if (pinstance_ == nullptr) {
		pinstance_ = new SplineRoad();
	}
	return pinstance_;
}

void SplineRoad::SetWorld(UWorld* World1) {
	World = World1;
	Init();
}

void SplineRoad::Init() {
	if (_roadsActor == nullptr || !IsValid(_roadsActor)) {
		FString name = "SplineRoads";
		ModelBase* modelBase = ModelBase::GetInstance();
		FActorSpawnParameters spawnParams;
		_roadsActor = modelBase->CreateActor(name, FVector(0,0,0), FVector(0,0,0), FVector(1,1,1), spawnParams);
	}
}

void SplineRoad::DestroyRoads() {
	for (auto& Elem1 : _RoadsByType) {
		FString type = Elem1.Key;
		for (auto& Elem : _RoadsByType[type]) {
			FString UName = Elem.Key;
			if (_RoadsActors.Contains(UName) && IsValid(_RoadsActors[UName])) {
				_RoadsActors[UName]->Destroy();
			}
		}
		Elem1.Value.Empty();
	}
	_RoadsByType.Empty();
	_RoadsActors.Empty();

	FString name = "SplineRoads";
	// Clear out any existing roads.
	UnrealGlobal* unrealGlobal = UnrealGlobal::GetInstance();
	AActor* actor = unrealGlobal->GetActorByName(name, AStaticMeshActor::StaticClass());
	if (actor) {
		unrealGlobal->RemoveAttachedActors(actor);
	}

	InstancedMesh* instancedMesh = InstancedMesh::GetInstance();
	instancedMesh->ClearInstances("RoadRoundabout");
}

void SplineRoad::CleanUp() {
	DestroyRoads();
	if (_roadsActor && IsValid(_roadsActor)) {
		_roadsActor->Destroy();
	}
	_roadsActor = nullptr;
	pinstance_ = nullptr;
}

void SplineRoad::AddRoads(TMap<FString, FPolygon> roadsPaths) {
	for (auto& Elem : roadsPaths) {
		FString UName = Elem.Key;
		FString type = Elem.Value.type;
		if (!_RoadsByType.Contains(type)) {
			_RoadsByType.Add(type, {});
		}
		if (!_RoadsByType[type].Contains(UName)) {
			_RoadsByType[type].Add(UName, Elem.Value);
		}
	}
}

void SplineRoad::DrawRoads() {
	Init();
	float flatteningMeters = 10;
	UnrealGlobal* unrealGlobal = UnrealGlobal::GetInstance();
	LoadContent* loadContent = LoadContent::GetInstance();
	HeightMap* heightMap = HeightMap::GetInstance();
	float widthMeters;
	FString UName, name, nameTemp, uNameRoundabout;
	TArray<FVector> vertices;
	int verticesCount;
	AActor* actor;
	USceneComponent* parent;
	UObject* parentObject;
	FRotator rotation = FRotator(0,0,0);
	FActorSpawnParameters spawnParams;
	FVector location = FVector(0,0,0);
	USceneComponent* roadsParent = _roadsActor->FindComponentByClass<USceneComponent>();
	// USplineMeshComponent* SplineMesh;
	FSplinePoint point;
	FVector direction, pointToUse;

	InstancedMesh* instancedMesh = InstancedMesh::GetInstance();
	PMBase* pmBase = PMBase::GetInstance();

	TArray<FString> roundaboutUNames = {};

	// TArray<FString> meshNamesBush = loadContent->GetMeshNamesByTypes({ "bush" });
	TArray<FString> meshNamesBush = { "hazel", "rhododendron" };
	TArray<FString> meshNamesTree = loadContent->GetMeshNamesByTypes({ "tree" });
	bool saveHeightMap = false;
	TMap<FString, FImagePixelValue> newHeightImageValues = {};

	FModelParams modelParams;
	modelParams.meshPath = loadContent->Mesh("roadSegment1");
	FModelCreateParams createParams;
	TMap<FString, FString> pairs;
	FString materialKey;
	int addPlants = 0;
	int carveLand = 1;

	for (auto& Elem1 : _RoadsByType) {
		FString type = Elem1.Key;

		FPlaceParams placeParamsNature = FPlaceParams();
		placeParamsNature.snapToGround = true;
		placeParamsNature.width = 5;
		float placingOffset = 5;

		for (auto& Elem : _RoadsByType[type]) {
			UName = Elem.Key;
			pairs = Lodash::PairsStringToObject(Elem.Value.pairsString);
			widthMeters = pairs.Contains("width") ? DataConvert::Float(pairs["width"]) : 3;
			addPlants = pairs.Contains("addPlants") ? 1 : 0;
			vertices = Elem.Value.vertices;
			materialKey = pairs.Contains("mat") ? pairs["mat"] : "asphalt";
			modelParams.materialPath = loadContent->Material(materialKey);

			spawnParams.Name = FName(UName);
			actor = (AActor*)World->SpawnActor<AActor>(
				AActor::StaticClass(), FVector(0,0,0) * unrealGlobal->GetScale(), FRotator(0,0,0), spawnParams);
			actor->SetActorLabel(UName);
			_RoadsActors.Add(UName, actor);
			parent = actor->FindComponentByClass<USceneComponent>();
			parentObject = (UObject*)actor;
			nameTemp = UName + "_Spline";
			USplineComponent* spline = NewObject<USplineComponent>(parentObject,
				USplineComponent::StaticClass(), *nameTemp);
			spline->CreationMethod = EComponentCreationMethod::Instance;
			spline->RegisterComponent();
			actor->SetRootComponent(spline);
			// Must be after has root component.
			actor->AttachToComponent(roadsParent, FAttachmentTransformRules::KeepRelativeTransform);

			spline->AttachToComponent(parent, FAttachmentTransformRules::KeepRelativeTransform);
			// Apparently defaults to 2 points, so remove them.
			spline->ClearSplinePoints();

			modelParams.parent = parent;
			createParams.parentActor = actor;
			createParams.parent = modelParams.parent;

			verticesCount = vertices.Num();
			int pointCount = 0;
			for (int vv = 0; vv < verticesCount; vv++) {
				// Add point before the vertex for flattening out to align at intersection.
				if (vv > 0) {
					direction = (vertices[(vv - 1)] - vertices[vv]).GetClampedToMaxSize(flatteningMeters);
					pointToUse = vertices[vv] + direction;
					// Set Z to the same to ensure flat.
					pointToUse.Z = vertices[vv].Z;
					// UE_LOG(LogTemp, Display, TEXT("pre-point %s prev %s cur %s"), *pointToUse.ToString(), *vertices[(vv-1)].ToString(), *vertices[vv].ToString());
					point = FSplinePoint((float)pointCount, pointToUse * unrealGlobal->GetScale());
					spline->AddPoint(point, false);
					pointCount += 1;
				}

				// UE_LOG(LogTemp, Display, TEXT("point %s"), *vertices[vv].ToString());
				point = FSplinePoint((float)pointCount, vertices[vv] * unrealGlobal->GetScale());
				// point = FSplinePoint((float)vv, vertices[vv] * unrealGlobal->GetScale());
				spline->AddPoint(point, false);
				pointCount += 1;

				// Add point after the vertex for flattening out to align at intersection.
				if (vv < verticesCount - 1) {
					direction = (vertices[(vv + 1)] - vertices[vv]).GetClampedToMaxSize(flatteningMeters);
					pointToUse = vertices[vv] + direction;
					// Set Z to the same to ensure flat.
					pointToUse.Z = vertices[vv].Z;
					// UE_LOG(LogTemp, Display, TEXT("post-point %s cur %s next %s"), *pointToUse.ToString(), *vertices[vv].ToString(), *vertices[(vv+1)].ToString());
					point = FSplinePoint((float)pointCount, pointToUse * unrealGlobal->GetScale());
					spline->AddPoint(point, false);
					pointCount += 1;
				}

				// Add roundabout.
				// 0 digits to block overlap within 1 meter.
				uNameRoundabout = "BuildingRoad_" + Lodash::ToFixed(vertices[vv].X, 0) + "_" + Lodash::ToFixed(vertices[vv].Y, 0);
				if (!roundaboutUNames.Contains(uNameRoundabout)) {
					// Move up a bit to cover roads.
					FVector pos = vertices[vv];
					pos.Z += 0.25;

					instancedMesh->CreateInstance("RoadRoundabout", pos, FRotator(0,Lodash::RandomRangeFloat(0,360),0), FVector(2,2,1.5));
					roundaboutUNames.Add(uNameRoundabout);
				}

				// Carve land (heightmap)
				if (carveLand > 0 && type == "road" && vv > 0) {
					newHeightImageValues = heightMap->CarveLine(vertices[(vv - 1)], vertices[vv], widthMeters,
						newHeightImageValues);
					saveHeightMap = true;
				}
			}

			// Must update to get tangents calculated, so do mesh at end after have all points.
			spline->UpdateSpline();
			FVector tangentStart, tangentEnd, pathLine;
			for (int ii = 1; ii < pointCount; ii++) {
				tangentStart = FVector(0,0,0);
				tangentEnd = FVector(0,0,0);
				// TODO - this did not seem to do anything; need to understand tangents better?
				// // Fix tangents at (next to) start and end by flattening them (z = 0)
				// if (ii == 2) {
				// 	pathLine = vertices[1] - vertices[0];
				// 	tangentStart = FVector(pathLine.X, pathLine.Y, 0);
				// } else if (ii == pointCount - 2) {
				// 	pathLine = vertices[(verticesCount - 1)] - vertices[(verticesCount - 2)];
				// 	tangentEnd = FVector(pathLine.X, pathLine.Y, 0);
				// }
				PMSpline::AddSplineMesh(UName, ii, modelParams, createParams, widthMeters, spline,
					tangentStart, tangentEnd);
			}

			if (addPlants > 0) {
				// Place nature / trees on sides of roads.
				placeParamsNature.spacing = 5;
				placeParamsNature.spacingCrossAxis = 2;
				placeParamsNature.scaleMin = 0.75;
				placeParamsNature.scaleMax = 1.25;
				LayoutPolyLine::PlaceOnLineSides(vertices, widthMeters + placingOffset * 2,
					meshNamesBush, placeParamsNature);
				placeParamsNature.spacing = 20;
				placeParamsNature.spacingCrossAxis = 999;
				placeParamsNature.scaleMin = 0.1;
				placeParamsNature.scaleMax = 0.5;
				LayoutPolyLine::PlaceOnLineSides(vertices, widthMeters + placingOffset * 2,
					meshNamesTree, placeParamsNature);
			}
		}
	}

	if (saveHeightMap) {
		// heightMap->SaveImage();
		heightMap->SetImageValues(newHeightImageValues);
	}
}
