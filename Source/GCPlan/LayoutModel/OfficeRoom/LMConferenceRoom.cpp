#include "LMConferenceRoom.h"

#include "../LayoutModelBase.h"
#include "../LMRoomPlants.h"
#include "../LMTableChairs.h"
#include "../../Common/DataConvert.h"
#include "../../Common/Lodash.h"
#include "../../Common/MathPolygon.h"
#include "../../Common/MathVector.h"
#include "../../Landscape/VerticesEdit.h"
#include "../../Layout/LayoutPlace.h"
#include "../../Layout/LayoutPolyLine.h"
#include "../../Mesh/LoadContent.h"
#include "../../Modeling/ModelBase.h"
#include "../../Modeling/Furniture/ModelTable.h"
#include "../../Mesh/DynamicMaterial.h"
#include "../../Mesh/LoadContent.h"
#include "../../ModelingStructsActor.h"
#include "../../BuildingStructsActor.h"

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
	plantParams.pairsStringPlants = "meshes=brackenFern,solidFern,cinnamonFern&placeOffsetAverage=0.3";
	LMRoomPlants::WallPlants(size, modelParams, createParamsIn, plantParams);

	// scale = FVector(size.X / 2, size.Y / 2, -1);
	scale = FVector(2.5 + 1 * 2, 5.5 + 1 * 2, 1);
	FTableChairs tableParams;
	tableParams.tableMeshes = { "conferenceTable" };
	tableParams.chairMeshes = { "officeChair" };
	LMTableChairs::TableWithChairs(scale, modelParams, createParamsIn, tableParams);

	VerticesEdit* verticesEdit = VerticesEdit::GetInstance();
	verticesEdit->AddAndSave(polygons);
	return polygons;
}
