#include "PMSpline.h"

#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"
#include "Engine/StaticMeshActor.h"

#include "../Common/Lodash.h"
#include "../Common/UnrealGlobal.h"
#include "../Modeling/ModelBase.h"
#include "PMBase.h"

PMSpline::PMSpline() {
}

PMSpline::~PMSpline() {
}

AActor* PMSpline::Create(FString name, TArray<FVector> vertices, float widthMeters,
	FModelCreateParams createParams, FModelParams modelParams) {
 
	ModelBase* modelBase = ModelBase::GetInstance();
	AActor* actor = modelBase->CreateActor(name + Lodash::GetInstanceId("_Spline"), -1 * createParams.offset);
	USceneComponent* parent = actor->FindComponentByClass<USceneComponent>();
	FModelParams clonerModelParams = modelParams;
	clonerModelParams.parent = parent;
	FModelCreateParams clonerCreateParams;
	clonerCreateParams.parentActor = actor;
	clonerCreateParams.parent = parent;
	UObject* parentObject = (UObject*)actor;

	FString nameTemp = name + "_Spline";
	USplineComponent* spline = NewObject<USplineComponent>(parentObject,
		USplineComponent::StaticClass(), *nameTemp);
	spline->CreationMethod = EComponentCreationMethod::Instance;
	spline->RegisterComponent();
	actor->SetRootComponent(spline);
	// Must be after has root component.
	actor->AttachToComponent(createParams.parent, FAttachmentTransformRules::KeepRelativeTransform);

	spline->AttachToComponent(parent, FAttachmentTransformRules::KeepRelativeTransform);
	// Apparently defaults to 2 points, so remove them.
	spline->ClearSplinePoints();

	UnrealGlobal* unrealGlobal = UnrealGlobal::GetInstance();
	int verticesCount = vertices.Num();
	int pointCount = 0;
	FSplinePoint point;
	for (int vv = 0; vv < verticesCount; vv++) {
		point = FSplinePoint((float)pointCount, vertices[vv] * unrealGlobal->GetScale());
		spline->AddPoint(point, false);
		pointCount += 1;
	}

	// Must update to get tangents calculated, so do mesh at end after have all points.
	spline->UpdateSpline();
	FVector tangentStart, tangentEnd, pathLine;
	for (int ii = 1; ii < pointCount; ii++) {
		tangentStart = FVector(0,0,0);
		tangentEnd = FVector(0,0,0);
		PMSpline::AddSplineMesh(name, ii, clonerModelParams, clonerCreateParams, widthMeters, spline,
			tangentStart, tangentEnd);
	}

	return actor;
}

void PMSpline::AddSplineMesh(FString UName, int pointCount, FModelParams modelParams,
	FModelCreateParams createParams, float widthMeters, USplineComponent* spline,
	FVector tangentStart, FVector tangentEnd) {
	// After have at least 2 points, add mesh between this point and past point.
	if (pointCount > 0) {
		FVector pointLocationStart, pointTangentStart, pointLocationEnd, pointTangentEnd;
		FString nameTemp = UName + "_SplineMesh_" + FString::FromInt(pointCount);
		USplineMeshComponent* SplineMesh = InitMesh(nameTemp, modelParams, createParams, widthMeters);
		spline->GetLocalLocationAndTangentAtSplinePoint((pointCount - 1), pointLocationStart, pointTangentStart);
		spline->GetLocalLocationAndTangentAtSplinePoint((pointCount), pointLocationEnd, pointTangentEnd);
		// UE_LOG(LogTemp, Display, TEXT("mesh tangents start %s end %s locStart %s locEnd %s"), *pointTangentStart.ToString(), *pointTangentEnd.ToString(), *pointLocationStart.ToString(), *pointLocationEnd.ToString());
		if (tangentStart != FVector(0,0,0)) {
			UE_LOG(LogTemp, Display, TEXT("tangentStart %s orig %s"), *tangentStart.ToString(), *pointTangentStart.ToString());
			pointTangentStart = tangentStart;
		}
		if (tangentEnd != FVector(0,0,0)) {
			UE_LOG(LogTemp, Display, TEXT("tangentEnd %s orig %s"), *tangentEnd.ToString(), *pointTangentEnd.ToString());
			pointTangentEnd = tangentEnd;
		}
		SplineMesh->SetStartAndEnd(pointLocationStart, pointTangentStart, pointLocationEnd, pointTangentEnd);
	}
}

USplineMeshComponent* PMSpline::InitMesh(FString nameTemp, FModelParams modelParams,
	FModelCreateParams createParams, float widthMeters, float widthMetersMesh) {
	UObject* parentObject = (UObject*)createParams.parentActor;
	USplineMeshComponent* SplineMesh = NewObject<USplineMeshComponent>(parentObject,
		USplineMeshComponent::StaticClass(), *nameTemp);
	SplineMesh->CreationMethod = EComponentCreationMethod::Instance;
	SplineMesh->RegisterComponent();
	SplineMesh->AttachToComponent(createParams.parent, FAttachmentTransformRules::KeepRelativeTransform);
	// SplineMesh->bCreatedByConstructionScript = true;
	SplineMesh->SetMobility(EComponentMobility::Movable);
	// SplineMesh->AttachParent = spline;
	SplineMesh->bCastDynamicShadow = false;

	ModelBase::SetMeshMaterialFromParams(SplineMesh, modelParams);

	// Width of the mesh
	float scale = widthMeters / widthMetersMesh;
	SplineMesh->SetStartScale(FVector2D(scale, scale));
	SplineMesh->SetEndScale(FVector2D(scale, scale));

	return SplineMesh;
}
