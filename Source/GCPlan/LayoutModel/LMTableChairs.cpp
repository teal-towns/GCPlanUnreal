#include "LMTableChairs.h"

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

LMTableChairs::LMTableChairs() {
}

LMTableChairs::~LMTableChairs() {
}

TMap<FString, FPolygon> LMTableChairs::TableWithChairs(FVector size, FModelParams modelParams,
	FModelCreateParams createParamsIn, FTableChairs params) {
	FVector rotation = FVector(0,0,0), location = FVector(0,0,0), scale = FVector(1,1,1);
	FString uName, type, pairsString, scaleString, meshKey;
	TArray<FVector> vertices;
	TMap<FString, FPolygon> polygons = {};
	LoadContent* loadContent = LoadContent::GetInstance();

	FVector tableLocation = FVector(0,0,0) + params.offset;
	uName = Lodash::GetInstanceId("Table");
	scale = FVector(size.X - params.chairWidth * 2, size.Y - params.chairWidth * 2, 1);
	FVector tableScale = scale;
	meshKey = params.tableMeshes[Lodash::RandomRangeInt(0, params.tableMeshes.Num() - 1)];
	pairsString = "mesh=" + meshKey + "&scale=" + loadContent->MeshScale(scale, meshKey) +
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
	placeParams.spacing = params.chairWidth;
	placeParams.spacingCrossAxis = 999;
	placeParams.spacingFactor = 0;
	placeParams.closedLoop = 1;
	placeParams.spacingStart = params.edgeSpacing;
	placeParams.spacingEnd = params.edgeSpacing;
	placeParams.alignCenter = 1;
	TMap<FString, FMeshTransform> transforms = LayoutPolyLine::PlaceOnLine(tableVertices, {}, placeParams);
	for (auto& Elem : transforms) {
		uName = Lodash::GetInstanceId("Chair");
		// Use as is (already rotated and offset from table vertices placing).
		meshKey = params.chairMeshes[Lodash::RandomRangeInt(0, params.chairMeshes.Num() - 1)];
		pairsString = "mesh=" + meshKey + "&rot=" + DataConvert::VectorToString(Elem.Value.rotation +
			loadContent->MeshRotation(meshKey));
		vertices = { Elem.Value.location };
		polygons.Add(uName, FPolygon(uName, uName, vertices, FVector(0,0,0), "chair", "point", pairsString));
	}

	VerticesEdit* verticesEdit = VerticesEdit::GetInstance();
	verticesEdit->AddAndSave(polygons);
	return polygons;
}
