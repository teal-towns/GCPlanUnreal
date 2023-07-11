#include "ModelBuilding.h"

#include "../ModelBase.h"
#include "../../Common/Lodash.h"
#include "../../Common/MathVector.h"
#include "../../ProceduralModel/PMPlanePolygon.h"

ModelBuilding::ModelBuilding() {
}

ModelBuilding::~ModelBuilding() {
}

AActor* ModelBuilding::Create(TArray<FVector> verticesGround, FModelParams modelParams,
	FModelCreateParams createParamsIn, FModelBuilding params) {
	ModelBase* modelBase = ModelBase::GetInstance();
	FString name = Lodash::GetInstanceId("Building_");
	FVector rotation = FVector(0,0,0), location = FVector(0,0,0), scale = FVector(1,1,1);
	AActor* actor = modelBase->CreateActorEmpty(name, modelParams);
	modelParams.parent = actor->FindComponentByClass<USceneComponent>();

	modelParams.meshKey = "cube";
	FActorSpawnParameters spawnParams;

	FVector path, nextVertex, vertex, midPoint;
	int nextVertexIndex;
	float distance, angle;
	for (int ii = 0; ii < verticesGround.Num(); ii++) {
		vertex = verticesGround[ii];
		nextVertexIndex = ii < verticesGround.Num() - 1 ? ii + 1 : 0;
		nextVertex = verticesGround[nextVertexIndex];
		path = nextVertex - vertex;
		distance = path.Size();
		midPoint = vertex + path.GetClampedToMaxSize(distance * 0.5);
		location = midPoint;
		scale = FVector(params.width, distance, params.height);
		angle = MathVector::SignedAngle(FVector(0,1,0), path, FVector(0,0,1));
		rotation = FVector(0,0,angle);
		modelParams.materialKey = params.materialsByVertex.Contains(ii) ? params.materialsByVertex[ii] : params.materialKeyDefault;
		modelBase->CreateActor(name + "_Wall_" + FString::FromInt(ii), location, rotation, scale, spawnParams, modelParams);
	}

	// Top
	FModelCreateParams createParams;
	createParams.parentActor = actor;
	createParams.parent = modelParams.parent;
	FPlanePolygon planeParams;
	planeParams.triangleDirection = params.triangleDirection;
	if (params.doTop) {
		createParams.offset = FVector(0,0,params.height);
		modelParams.materialKey = params.topMaterial;
		PMPlanePolygon::Create(verticesGround, createParams, modelParams, planeParams);
	}
	// Bottom
	if (params.doBottom) {
		createParams.offset = FVector(0,0,0);
		modelParams.materialKey = params.bottomMaterial;
		PMPlanePolygon::Create(verticesGround, createParams, modelParams, planeParams);
	}

	ModelBase::SetTransformFromParams(actor, createParamsIn);
	return actor;
}
