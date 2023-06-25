#include "LMKitchen.h"

#include "LayoutModelBase.h"
#include "LMRoomPlants.h"
#include "../Common/DataConvert.h"
#include "../Common/Lodash.h"
#include "../Common/MathPolygon.h"
#include "../Common/MathVector.h"
#include "../Landscape/VerticesEdit.h"
#include "../Layout/LayoutPlace.h"
#include "../Layout/LayoutPolyLine.h"
#include "../Mesh/LoadContent.h"
#include "../Modeling/ModelBase.h"
#include "../Modeling/Furniture/ModelTable.h"
#include "../Mesh/DynamicMaterial.h"
#include "../Mesh/LoadContent.h"
#include "../ModelingStructsActor.h"
#include "../BuildingStructsActor.h"

LMKitchen::LMKitchen() {
}

LMKitchen::~LMKitchen() {
}

TMap<FString, FPolygon> LMKitchen::OfficeWall(FVector size, FModelParams modelParams,
	FModelCreateParams createParamsIn, FKitchen params) {
	FVector rotation = FVector(0,0,0), location = FVector(0,0,0), scale = FVector(1,1,1);
	FString uName, type, pairsString, scaleString, meshKey;
	TArray<FVector> vertices;
	TMap<FString, FPolygon> polygons = {};
	LoadContent* loadContent = LoadContent::GetInstance();
	FString uNameBase = Lodash::GetInstanceId("Kitchen");

	// In front facing backward
	FVector size1;
	float currentY = size.Y / -2;
	float currentX = size.X / 2;
	uName = uNameBase + "_sinks";
	meshKey = "industrialSinks";
	size1 = loadContent->GetMeshSize(meshKey);
	location = FVector(currentX - size1.X / 2, currentY + size1.Y / 2, 0);
	rotation = FVector(0,0,180);
	pairsString = "mesh=" + meshKey + ModelBase::AddRotationString(createParamsIn.rotation, rotation, meshKey);
	vertices = { MathVector::RotateVector(location + params.offset, createParamsIn.rotation) + createParamsIn.offset };
	vertices = ModelBase::Vertices(vertices, createParamsIn, params.rotation);
	polygons.Add(uName, FPolygon(uName, uName, vertices, FVector(0,0,0), "kitchen", "point", pairsString));
	currentY += size1.Y;

	uName = uNameBase + "_cabinet";
	meshKey = "industrialCabinet";
	size1 = loadContent->GetMeshSize(meshKey);
	location = FVector(currentX - size1.X / 2, currentY + size1.Y / 2, 0);
	rotation = FVector(0,0,180);
	pairsString = "mesh=" + meshKey + ModelBase::AddRotationString(createParamsIn.rotation, rotation, meshKey);
	vertices = { MathVector::RotateVector(location + params.offset, createParamsIn.rotation) + createParamsIn.offset };
	vertices = ModelBase::Vertices(vertices, createParamsIn, params.rotation);
	polygons.Add(uName, FPolygon(uName, uName, vertices, FVector(0,0,0), "kitchen", "point", pairsString));
	currentY += size1.Y;

	uName = uNameBase + "_shelves";
	meshKey = "shelves";
	size1 = loadContent->GetMeshSize(meshKey);
	location = FVector(currentX - size1.X / 2, currentY + size1.Y / 2, 0);
	rotation = FVector(0,0,180);
	pairsString = "mesh=" + meshKey + ModelBase::AddRotationString(createParamsIn.rotation, rotation, meshKey);
	vertices = { MathVector::RotateVector(location + params.offset, createParamsIn.rotation) + createParamsIn.offset };
	vertices = ModelBase::Vertices(vertices, createParamsIn, params.rotation);
	polygons.Add(uName, FPolygon(uName, uName, vertices, FVector(0,0,0), "kitchen", "point", pairsString));
	currentY += size1.Y;

	VerticesEdit* verticesEdit = VerticesEdit::GetInstance();
	verticesEdit->AddAndSave(polygons);
	return polygons;
}
