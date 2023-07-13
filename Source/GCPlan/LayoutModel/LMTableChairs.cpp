#include "LMTableChairs.h"

#include "LayoutModelBase.h"
#include "LMRoomPlants.h"
#include "../Common/DataConvert.h"
#include "../Common/Lodash.h"
#include "../Common/MathPolygon.h"
#include "../Common/MathVector.h"
#include "../Landscape/VerticesEdit.h"
#include "../Layout/LayoutPlace.h"
#include "../Layout/LayoutPolygon.h"
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

	auto [valid, meshesByTags] = loadContent->FillMeshesByTags(params.meshesByTags, { "table", "chair" });
	if (!valid) {
		UE_LOG(LogTemp, Warning, TEXT("LMTableChairs.TableWithChairs missing meshesByTags, skipping"));
		return polygons;
	}
	params.meshesByTags = meshesByTags;

	FVector tableLocation = FVector(0,0,0) + params.offset;
	uName = Lodash::GetInstanceId("Table");
	scale = FVector(size.X - params.chairWidth * 2, size.Y - params.chairWidth * 2, 1);
	FVector tableScale = scale;
	meshKey = params.meshesByTags["table"][Lodash::RandomRangeInt(0, params.meshesByTags["table"].Num() - 1)];
	pairsString = "mesh=" + meshKey + "&scale=" + loadContent->MeshScale(scale, meshKey, {"x","y"}) +
		ModelBase::AddRotationString(createParamsIn.rotation);
	vertices = { MathVector::RotateVector(tableLocation, createParamsIn.rotation) + createParamsIn.offset };
	vertices = ModelBase::Vertices(vertices, createParamsIn);
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
		meshKey = params.meshesByTags["chair"][Lodash::RandomRangeInt(0, params.meshesByTags["chair"].Num() - 1)];
		pairsString = "mesh=" + meshKey + "&rot=" + DataConvert::VectorToString(Elem.Value.rotation +
			loadContent->MeshRotation(meshKey));
		vertices = { Elem.Value.location };
		vertices = ModelBase::Vertices(vertices, createParamsIn);
		polygons.Add(uName, FPolygon(uName, uName, vertices, FVector(0,0,0), "chair", "point", pairsString));
	}

	VerticesEdit* verticesEdit = VerticesEdit::GetInstance();
	verticesEdit->AddAndSave(polygons);
	return polygons;
}

TMap<FString, FPolygon> LMTableChairs::TablesAndChairs(FVector size, FModelParams modelParams,
	FModelCreateParams createParamsIn, FTableChairs params) {
	FVector rotation = FVector(0,0,0), location = FVector(0,0,0), scale = FVector(1,1,1);
	FString uName, type, pairsString, scaleString, meshKey;
	TArray<FVector> vertices;
	TMap<FString, FPolygon> polygons = {};
	LoadContent* loadContent = LoadContent::GetInstance();

	FPlaceParams placeParams;
	placeParams.skipMesh = true;
	placeParams.closedLoop = 1;
	placeParams.offsetAverage = params.tableWidth + params.chairWidth * 2;
	placeParams.offsetMaxFactorX = params.placeOffsetMaxYFactor;
	placeParams.offsetMaxFactorY = params.placeOffsetMaxYFactor;
	placeParams.offsetX = params.placeOffsetX;
	placeParams.offsetY = params.placeOffsetY;
	float buffer = params.tableWidth + params.chairWidth;
	FVector range = size - FVector(buffer / 2, buffer / 2, 0);
	vertices = MathPolygon::PointToBox(FVector(0,0,0), range);
	TMap<FString, FMeshTransform> transforms = LayoutPolygon::PlaceInPolygon(vertices, {}, placeParams);
	scale = FVector(params.tableWidth + params.chairWidth + params.chairWidth / 2,
		params.tableWidth + params.chairWidth + params.chairWidth / 2, size.Z);
	FVector offsetBase = params.offset;
	bool placedPeople = false;
	float chairPersonProbabilityOriginal = params.chairPersonProbability;
	for (auto& Elem : transforms) {
		params.offset = offsetBase + Elem.Value.location;
		if (!placedPeople && Lodash::RandomRangeFloat(0,1) <= params.chairPersonProbability) {
			placedPeople = true;
			params.chairPersonProbability = 1;
		} else {
			params.chairPersonProbability = 0;
		}
		SmallTableChairs(scale, modelParams, createParamsIn, params);
		// Reset.
		params.chairPersonProbability = chairPersonProbabilityOriginal;
	}

	VerticesEdit* verticesEdit = VerticesEdit::GetInstance();
	verticesEdit->AddAndSave(polygons);
	return polygons;
}

TMap<FString, FPolygon> LMTableChairs::SmallTableChairs(FVector size, FModelParams modelParams,
	FModelCreateParams createParamsIn, FTableChairs params) {
	FVector rotation = FVector(0,0,0), location = FVector(0,0,0), scale = FVector(1,1,1);
	FString uName, type, pairsString, scaleString, meshKey;
	TArray<FVector> vertices;
	TMap<FString, FPolygon> polygons = {};
	LoadContent* loadContent = LoadContent::GetInstance();
	FString uNameBase = Lodash::GetInstanceId("SmallTableChairs");

	auto [valid, meshesByTags] = loadContent->FillMeshesByTags(params.meshesByTags, { "table", "chair" });
	if (!valid) {
		UE_LOG(LogTemp, Warning, TEXT("LMTableChairs.SmallTableChairs missing meshesByTags, skipping"));
		return polygons;
	}
	params.meshesByTags = meshesByTags;

	// Table in center
	uName = uNameBase + "_table";
	scale = FVector(params.tableWidth, params.tableWidth, params.tableHeight);
	location = FVector(0,0,0);
	FVector tableScale = scale;
	FVector tableLocation = location;
	meshKey = params.meshesByTags["table"][Lodash::RandomRangeInt(0, params.meshesByTags["table"].Num() - 1)];
	pairsString = "mesh=" + meshKey + "&scale=" + DataConvert::VectorToString(scale) +
		ModelBase::AddRotationString(createParamsIn.rotation, params.rotation, meshKey);
	vertices = { MathVector::RotateVector(location + params.offset, createParamsIn.rotation) + createParamsIn.offset };
	vertices = ModelBase::Vertices(vertices, createParamsIn, params.rotation);
	polygons.Add(uName, FPolygon(uName, uName, vertices, FVector(0,0,0), "table", "point", pairsString));

	// Do not place the same person twice.
	int personIndex = 0;
	FVector personOffset;
	float personZ = 0.02;
	float personX = 0.2;
	// Chairs on side
	for (int ii = 0; ii < params.chairPositions.Num(); ii++) {
		uName = uNameBase + Lodash::GetInstanceId("_chair");
		if (params.chairPositions[ii] == "front") {
			location = tableLocation + FVector(tableScale.X / 2 + params.chairWidth / 2,0,0);
			rotation = FVector(0,0,-180);
			personOffset = FVector(-1 * personX, 0, personZ);
		} else if (params.chairPositions[ii] == "back") {
			location = tableLocation + FVector(tableScale.X / -2  - params.chairWidth / 2,0,0);
			rotation = FVector(0,0,0);
			personOffset = FVector(personX, 0, personZ);
		} else if (params.chairPositions[ii] == "right") {
			location = tableLocation + FVector(0,tableScale.Y / 2  + params.chairWidth / 2,0);
			rotation = FVector(0,0,-90);
			personOffset = FVector(0, -1 * personX, personZ);
		} else if (params.chairPositions[ii] == "left") {
			location = tableLocation + FVector(0,tableScale.Y / -2  - params.chairWidth / 2,0);
			rotation = FVector(0,0,90);
			personOffset = FVector(0, personX, personZ);
		}
		meshKey = params.meshesByTags["chair"][Lodash::RandomRangeInt(0, params.meshesByTags["chair"].Num() - 1)];
		pairsString = "mesh=" + meshKey +
			ModelBase::AddRotationString(createParamsIn.rotation, rotation + params.rotation, meshKey);
		vertices = { MathVector::RotateVector(location + params.offset, createParamsIn.rotation) + createParamsIn.offset };
		vertices = ModelBase::Vertices(vertices, createParamsIn, params.rotation);
		polygons.Add(uName, FPolygon(uName, uName, vertices, FVector(0,0,0), "chair", "point", pairsString));

		if (Lodash::RandomRangeFloat(0,1) <= params.chairPersonProbability &&
			personIndex < params.meshesByTags["people"].Num()) {
			uName = uNameBase + Lodash::GetInstanceId("_person");
			meshKey = params.meshesByTags["people"][personIndex];
			location += personOffset;
			personIndex += 1;
			pairsString = "mesh=" + meshKey +
				ModelBase::AddRotationString(createParamsIn.rotation, rotation + params.rotation, meshKey);
			vertices = { MathVector::RotateVector(location + params.offset, createParamsIn.rotation) + createParamsIn.offset };
			vertices = ModelBase::Vertices(vertices, createParamsIn, params.rotation);
			polygons.Add(uName, FPolygon(uName, uName, vertices, FVector(0,0,0), "person", "point", pairsString));
		}
	}

	// table items
	if (params.meshesByTags["flower"].Num() > 0 && Lodash::RandomRangeFloat(0,1) <= params.tableItemsProbability) {
		// planter box
		uName = uNameBase + "_planterBox";
		location = tableLocation;
		scale = FVector(0.15, 0.15, 0.2);
		FVector planterScale = scale;
		pairsString = "meshRule=planterBox&scale=" + DataConvert::VectorToString(scale) +
			ModelBase::AddRotationString(createParamsIn.rotation);
		vertices = { MathVector::RotateVector(location + params.offset, createParamsIn.rotation) + createParamsIn.offset };
		vertices = ModelBase::Vertices(vertices, createParamsIn);
		polygons.Add(uName, FPolygon(uName, uName, vertices, FVector(0,0,0), "planterBox", "point", pairsString));
		// flower
		uName = uNameBase + "_flower";
		location = tableLocation + FVector(0,0,planterScale.Z - 0.02);
		meshKey = params.meshesByTags["flower"][Lodash::RandomRangeInt(0, params.meshesByTags["flower"].Num() - 1)];
		pairsString = "mesh=" + meshKey + ModelBase::AddRotationString(createParamsIn.rotation, rotation, meshKey);
		vertices = { MathVector::RotateVector(location + params.offset, createParamsIn.rotation) + createParamsIn.offset };
		vertices = ModelBase::Vertices(vertices, createParamsIn, params.rotation);
		polygons.Add(uName, FPolygon(uName, uName, vertices, FVector(0,0,0), "flower", "point", pairsString));
	}

	VerticesEdit* verticesEdit = VerticesEdit::GetInstance();
	verticesEdit->AddAndSave(polygons);
	return polygons;
}
