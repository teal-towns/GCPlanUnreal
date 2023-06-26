#pragma once

#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"

#include "PMBase.h"
#include "../Modeling/ModelBase.h"

class PMSpline {
public:
	PMSpline();
	~PMSpline();

	static AActor* Create(FString name, TArray<FVector> vertices, float widthMeters,
		FModelCreateParams createParams, FModelParams modelParams);
	static void AddSplineMesh(FString UName, int pointCount, FModelParams modelParams,
		FModelCreateParams createParams, float widthMeters, USplineComponent* spline,
		FVector tangentStart = FVector(0,0,0), FVector tangentEnd = FVector(0,0,0));
	static USplineMeshComponent* InitMesh(FString nameTemp, FModelParams modelParams,
		FModelCreateParams createParams, float widthMeters, float widthMetersMesh = 10);
};
