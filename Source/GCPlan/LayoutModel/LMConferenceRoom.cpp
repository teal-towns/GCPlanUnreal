#include "LMConferenceRoom.h"

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

LMConferenceRoom::LMConferenceRoom() {
}

LMConferenceRoom::~LMConferenceRoom() {
}

TMap<FString, FPolygon> LMConferenceRoom::Create(FVector size, FModelParams modelParams,
	FModelCreateParams createParamsIn) {
	FVector rotation = FVector(0,0,0), location = FVector(0,0,0), scale = FVector(1,1,1);
	FString uName, type, pairsString, scaleString;
	TArray<FVector> vertices;
	TMap<FString, FPolygon> polygons = {};
	uName = Lodash::GetInstanceId("Room");
	pairsString = "meshRule=roomCube&mat=wood&bottomMat=marbleTile&scale=" + DataConvert::VectorToString(size) +
		ModelBase::AddRotationString(createParamsIn.rotation);
	vertices = { createParamsIn.offset };
	polygons.Add(uName, FPolygon(uName, uName, vertices, FVector(0,0,0), "room", "point", pairsString));

	FWallPlants plantParams;
	plantParams.pairsStringPlants = "meshes=fern,solidFern,cinnamonFern&placeOffsetAverage=0.3";
	LMRoomPlants::WallPlants(size, modelParams, createParamsIn, plantParams);

	// scale = FVector(size.X / 2, size.Y / 2, -1);
	scale = FVector(2.5 + 1 * 2, 5.5 + 1 * 2, -1);
	LMConferenceRoom::TableWithChairs(scale, modelParams, createParamsIn);

	VerticesEdit* verticesEdit = VerticesEdit::GetInstance();
	verticesEdit->AddAndSave(polygons);
	return polygons;
}

TMap<FString, FPolygon> LMConferenceRoom::TableWithChairs(FVector size, FModelParams modelParams,
	FModelCreateParams createParamsIn) {
	FVector rotation = FVector(0,0,0), location = FVector(0,0,0), scale = FVector(1,1,1);
	FString uName, type, pairsString, scaleString;
	TArray<FVector> vertices;
	TMap<FString, FPolygon> polygons = {};
	LoadContent* loadContent = LoadContent::GetInstance();

	float chairWidth = 1;
	FVector tableLocation = FVector(0,0,0);
	uName = Lodash::GetInstanceId("Table");
	scale = FVector(size.X - chairWidth * 2, size.Y - chairWidth * 2, 1);
	FVector tableScale = scale;
	pairsString = "mesh=conferenceTable&scale=" + loadContent->MeshScale(scale, "conferenceTable") +
		ModelBase::AddRotationString(createParamsIn.rotation);
	vertices = { MathVector::RotateVector(tableLocation, createParamsIn.rotation) + createParamsIn.offset };
	polygons.Add(uName, FPolygon(uName, uName, vertices, FVector(0,0,0), "table", "point", pairsString));

	// Go around table and place chairs.
	// Note, must use position BEFORE offset (then rotate after), otherwise rotation will be (way) off.
	TArray<FVector> tableVertices = MathPolygon::PointToBox(tableLocation, tableScale);
	tableVertices = MathVector::RotateVertices(tableVertices, createParamsIn.rotation, createParamsIn.offset);
	FVector center = MathPolygon::GetPolygonCenter(tableVertices);
	tableVertices = MathPolygon::BufferVertices(tableVertices, center, 1);
	FPlaceParams placeParams;
	placeParams.skipMesh = true;
	placeParams.rotateTowardCenter = true;
	placeParams.spacing = chairWidth;
	placeParams.spacingCrossAxis = 999;
	placeParams.spacingFactor = 0;
	placeParams.closedLoop = 1;
	placeParams.spacingStart = 1;
	placeParams.spacingEnd = 1;
	placeParams.spacingCenter = 1;
	TMap<FString, FMeshTransform> transforms = LayoutPolyLine::PlaceOnLine(tableVertices, {}, placeParams);
	for (auto& Elem : transforms) {
		uName = Lodash::GetInstanceId("OfficeChair");
		// Use as is (already rotated and offset from table vertices placing).
		pairsString = "mesh=officeChair&rot=" + DataConvert::VectorToString(Elem.Value.rotation +
			loadContent->MeshRotation("officeChair"));
		vertices = { Elem.Value.location };
		polygons.Add(uName, FPolygon(uName, uName, vertices, FVector(0,0,0), "chair", "point", pairsString));
	}

	VerticesEdit* verticesEdit = VerticesEdit::GetInstance();
	verticesEdit->AddAndSave(polygons);
	return polygons;
}
