#include "LMServerRoom.h"

#include "../LayoutModelBase.h"
#include "../LMKitchen.h"
#include "../LMRoomPlants.h"
#include "../LMTableChairs.h"
#include "../../Common/DataConvert.h"
#include "../../Common/Lodash.h"
#include "../../Common/MathPolygon.h"
#include "../../Common/MathVector.h"
#include "../../Landscape/VerticesEdit.h"
#include "../../Layout/LayoutPlace.h"
#include "../../Layout/LayoutPolygon.h"
#include "../../Mesh/LoadContent.h"
#include "../../Modeling/ModelBase.h"
#include "../../Modeling/Furniture/ModelTable.h"
#include "../../Mesh/DynamicMaterial.h"
#include "../../Mesh/LoadContent.h"
#include "../../ModelingStructsActor.h"
#include "../../BuildingStructsActor.h"

LMServerRoom::LMServerRoom() {
}

LMServerRoom::~LMServerRoom() {
}

TMap<FString, FPolygon> LMServerRoom::Create(FVector size, FModelParams modelParams,
	FModelCreateParams createParamsIn) {
	FVector rotation = FVector(0,0,0), location = FVector(0,0,0), scale = FVector(1,1,1);
	FString uName, type, pairsString, scaleString, meshKey;
	TArray<FVector> vertices;
	TMap<FString, FPolygon> polygons = {};
	LoadContent* loadContent = LoadContent::GetInstance();

	uName = Lodash::GetInstanceId("Room");
	pairsString = "meshRule=roomCube&mat=white&bottomMat=black&scale=" + DataConvert::VectorToString(size) +
		ModelBase::AddRotationString(createParamsIn.rotation);
	vertices = { createParamsIn.offset };
	polygons.Add(uName, FPolygon(uName, uName, vertices, FVector(0,0,0), "room", "point", pairsString));

	meshKey = "computerServer";
	FVector size1 = loadContent->GetMeshSize(meshKey);
	FVector serverSize = size1;

	FPlaceParams placeParams;
	placeParams.skipMesh = true;
	placeParams.offsetMaxFactorX = 0;
	placeParams.offsetMaxFactorY = 0;
	// 2 servers back to back with 1 meter spacing between rows.
	placeParams.offsetX = size1.X * 2 + 1;
	placeParams.offsetY = size1.Y;

	float buffer = 1;
	FVector offset = FVector(buffer * 2, 2 + buffer * 2, 0);
	location = FVector(0, 1, 0);
	size1 = size - offset;
	vertices = MathPolygon::PointToBox(location, size1);
	TMap<FString, FMeshTransform> transforms = LayoutPolygon::PlaceInPolygon(vertices, {}, placeParams);
	for (auto& Elem : transforms) {
		// 2 servers for each point, back to back.
		uName = Lodash::GetInstanceId("server");
		location = Elem.Value.location + FVector(serverSize.X / 2,0,0);
		rotation = FVector(0,0,0);
		pairsString = "mesh=" + meshKey +
			ModelBase::AddRotationString(createParamsIn.rotation, rotation, meshKey);
		vertices = { MathVector::RotateVector(location, createParamsIn.rotation) + createParamsIn.offset };
		vertices = ModelBase::Vertices(vertices, createParamsIn);
		polygons.Add(uName, FPolygon(uName, uName, vertices, FVector(0,0,0), "computerServer", "point", pairsString));

		uName = Lodash::GetInstanceId("server");
		location = Elem.Value.location + FVector(serverSize.X / -2,0,0);
		rotation = FVector(0,0,180);
		pairsString = "mesh=" + meshKey +
			ModelBase::AddRotationString(createParamsIn.rotation, rotation, meshKey);
		vertices = { MathVector::RotateVector(location, createParamsIn.rotation) + createParamsIn.offset };
		vertices = ModelBase::Vertices(vertices, createParamsIn);
		polygons.Add(uName, FPolygon(uName, uName, vertices, FVector(0,0,0), "computerServer", "point", pairsString));
	}

	VerticesEdit* verticesEdit = VerticesEdit::GetInstance();
	verticesEdit->AddAndSave(polygons);
	return polygons;
}
