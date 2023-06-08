#include "SplineRoad.h"

#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"
#include "Engine/StaticMeshActor.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "ProceduralMeshComponent.h"

#include "../Common/Lodash.h"
#include "../Common/UnrealGlobal.h"
#include "../Mesh/InstancedMesh.h"
#include "../ProceduralModel/PMBase.h"

SplineRoad* SplineRoad::pinstance_{nullptr};
std::mutex SplineRoad::mutex_;
AStaticMeshActor* SplineRoad::_roadsActor;

SplineRoad::SplineRoad() {
}

SplineRoad::~SplineRoad() {
}

SplineRoad *SplineRoad::GetInstance() {
	std::lock_guard<std::mutex> lock(mutex_);
	if (pinstance_ == nullptr) {
		pinstance_ = new SplineRoad();

		// In case of recompile in editor, will lose reference so need to check scene too.
		FString name = "SplineRoads";
		// UnrealGlobal* unrealGlobal = UnrealGlobal::GetInstance();
		// AActor* actor = unrealGlobal->GetActorByName(name, AStaticMeshActor::StaticClass());
		// if (actor) {
		// 	_roadsActor = actor;
		// } else {
			PMBase* pmBase = PMBase::GetInstance();
			FActorSpawnParameters spawnParams;
			_roadsActor = pmBase->CreateActor(name, FVector(0,0,0), FRotator(0,0,0), spawnParams);
		// }
	}
	return pinstance_;
}

void SplineRoad::SetWorld(UWorld* World1) {
	World = World1;
}

void SplineRoad::DestroyRoads() {
	for (auto& Elem1 : _RoadsByType) {
		FString type = Elem1.Key;
		for (auto& Elem : _RoadsByType[type]) {
			FString UName = Elem.Key;
			if (_RoadsActors.Contains(UName)) {
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

void SplineRoad::AddRoads(TMap<FString, FRoadPath> roadsPaths) {
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
	float flatteningMeters = 10;
	UnrealGlobal* unrealGlobal = UnrealGlobal::GetInstance();
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

	for (auto& Elem1 : _RoadsByType) {
		FString type = Elem1.Key;
		for (auto& Elem : _RoadsByType[type]) {
			UName = Elem.Key;
			widthMeters = Elem.Value.widthMeters;
			vertices = Elem.Value.vertices;

			// actor = pmBase->CreateActor(UName, location, rotation, spawnParams, roadsParent);
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
			spline->RegisterComponent();
			actor->SetRootComponent(spline);
			// Must be after has root component.
			actor->AttachToComponent(roadsParent, FAttachmentTransformRules::KeepRelativeTransform);

			spline->AttachToComponent(parent, FAttachmentTransformRules::KeepRelativeTransform);
			// Apparently defaults to 2 points, so remove them.
			spline->ClearSplinePoints();

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
					// AddSplineMesh(UName, pointCount, parentObject, parent, widthMeters, spline);
					pointCount += 1;
				}

				// UE_LOG(LogTemp, Display, TEXT("point %s"), *vertices[vv].ToString());
				point = FSplinePoint((float)pointCount, vertices[vv] * unrealGlobal->GetScale());
				// point = FSplinePoint((float)vv, vertices[vv] * unrealGlobal->GetScale());
				spline->AddPoint(point, false);
				// AddSplineMesh(UName, pointCount, parentObject, parent, widthMeters, spline);
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
					// AddSplineMesh(UName, pointCount, parentObject, parent, widthMeters, spline);
					pointCount += 1;
				}

				// Add roundabout.
				uNameRoundabout = "BuildingRoad_" + Lodash::ToFixed(vertices[vv].X, 1) + "_" + Lodash::ToFixed(vertices[vv].Y, 1);
				if (!roundaboutUNames.Contains(uNameRoundabout)) {
					// Move up a bit to cover roads.
					FVector pos = vertices[vv];
					pos.Z += 0.25;
					instancedMesh->CreateInstance("RoadRoundabout", pos, FRotator(0,0,0), FVector(2,2,1));
					roundaboutUNames.Add(uNameRoundabout);
				}
			}

			// Must update to get tangents calculated, so do mesh at end after have all points.
			spline->UpdateSpline();
			for (int ii = 1; ii < pointCount; ii++) {
				AddSplineMesh(UName, ii, parentObject, parent, widthMeters, spline);
			}
		}
	}
}

void SplineRoad::AddSplineMesh(FString UName, int pointCount, UObject* parentObject, USceneComponent* parent, float widthMeters,
	USplineComponent* spline) {
	// After have at least 2 points, add mesh between this point and past point.
	if (pointCount > 0) {
		FVector pointLocationStart, pointTangentStart, pointLocationEnd, pointTangentEnd;
		FString nameTemp = UName + "_SplineMesh_" + FString::FromInt(pointCount);
		USplineMeshComponent* SplineMesh = InitMesh(nameTemp, parentObject, parent, widthMeters);
		spline->GetLocalLocationAndTangentAtSplinePoint((pointCount - 1), pointLocationStart, pointTangentStart);
		spline->GetLocalLocationAndTangentAtSplinePoint((pointCount), pointLocationEnd, pointTangentEnd);
		// UE_LOG(LogTemp, Display, TEXT("mesh tangents start %s end %s locStart %s locEnd %s"), *pointTangentStart.ToString(), *pointTangentEnd.ToString(), *pointLocationStart.ToString(), *pointLocationEnd.ToString());
		SplineMesh->SetStartAndEnd(pointLocationStart, pointTangentStart, pointLocationEnd, pointTangentEnd);
	}
}

USplineMeshComponent* SplineRoad::InitMesh(FString nameTemp, UObject* parentObject, USceneComponent* parent, float widthMeters) {
	USplineMeshComponent* SplineMesh = NewObject<USplineMeshComponent>(parentObject,
		USplineMeshComponent::StaticClass(), *nameTemp);
	SplineMesh->RegisterComponent();
	SplineMesh->AttachToComponent(parent, FAttachmentTransformRules::KeepRelativeTransform);
	// SplineMesh->bCreatedByConstructionScript = true;
	SplineMesh->SetMobility(EComponentMobility::Movable);
	// SplineMesh->AttachParent = spline;
	SplineMesh->bCastDynamicShadow = false;

	FString meshPath = "/Script/Engine.StaticMesh'/Game/Landscape/RoadSegment1.RoadSegment1'";
	UStaticMesh* mesh = Cast<UStaticMesh>(StaticLoadObject(UStaticMesh::StaticClass(), NULL, *meshPath));
	SplineMesh->SetStaticMesh(mesh);

	FString materialPath = "/Script/Engine.Material'/Game/Landscape/Asphalt_M.Asphalt_M'";
	UMaterial* material = Cast<UMaterial>(StaticLoadObject(UMaterial::StaticClass(), NULL,
		*materialPath));
	SplineMesh->SetMaterial(0, material);
	// UMaterialInstanceDynamic* dynamicMat = UMaterialInstanceDynamic::Create(mSplineMeshMaterial, NULL);
	// dynamicMat->SetVectorParameterValue(TEXT("Color"), FLinearColor(mSegments[i].mColor));
	// SplineMesh->SetMaterial(0, dynamicMat);

	// Width of the mesh
	float scale = 1;
	SplineMesh->SetStartScale(FVector2D(scale, scale));
	SplineMesh->SetEndScale(FVector2D(scale, scale));

	return SplineMesh;
}
