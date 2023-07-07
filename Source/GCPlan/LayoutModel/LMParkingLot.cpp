#include "LMParkingLot.h"
#include "LayoutModelBase.h"
#include "../Building/BuildingRoom.h"
#include "../Common/DataConvert.h"
#include "../Common/Lodash.h"
#include "../Common/MathPolygon.h"
#include "../Common/MathVector.h"
#include "../Landscape/VerticesEdit.h"
#include "../Modeling/ModelBase.h"
#include "../Mesh/DynamicMaterial.h"
#include "../Mesh/LoadContent.h"
#include "../ModelingStructsActor.h"
#include "../BuildingStructsActor.h"

LMParkingLot::LMParkingLot()
{
}

LMParkingLot::~LMParkingLot()
{
}

TMap<FString, FPolygon> LMParkingLot::CreateFromInputs() {
	ModelBase *modelBase = ModelBase::GetInstance();
	LayoutModelBase *layoutModelBase = LayoutModelBase::GetInstance();
	auto [layoutParams, modelParams] = layoutModelBase->GetInputs("ParkingLot", FVector(150, 50, 0));
	FString name = layoutParams.name;
	FVector size = layoutParams.size;
	return Create(size, modelParams, FModelCreateParams());
}

TMap<FString, FPolygon> LMParkingLot::Create(FVector size, FModelParams modelParams,
	FModelCreateParams createParamsIn, FParkingLot params) {
	FVector rotation = FVector(0,0,0), location = FVector(0,0,0), scale = FVector(1,1,1);
	FString uName, type, pairsString, scaleString;
	TArray<FVector> vertices;
	TMap<FString, FPolygon> polygons = {};

	// Resize to section lengths.
	size.X = float(floor(size.X / 22) * 22);

	float parkingStripDepth = 7;
	// float treeStripDepth = 1;
	// float rowGap = 15;
	float treeStripDepth = 0;
	float rowGap = 1;
	int rowCount = floor((size.Y + rowGap) / (parkingStripDepth + treeStripDepth + rowGap));
	if (rowCount > 2) {
		rowCount = 2;
	}
	float treeGap = 15;
	int treeCountInRow = floor(size.X / treeGap);
	float centerY;
	// FModelCreateParams createParamsRow;
	// createParamsRow.rotation = createParamsIn.rotation;

	float rotationZ;
	FVector offsetBase = params.offset;
	FVector rotationBase = params.rotation;
	FVector rotateAroundBase = createParamsIn.rotateAround;

	for (int ii = 0; ii < rowCount; ii++)
	{
		centerY = parkingStripDepth / 2 + treeStripDepth / 2;
		// if (ii % 2 == 1) {
		// rotate around will handle updating location.
		// if (true) {
		// 	centerY *= -1;
		// }
		rotationZ = ii % 2 == 0 ? 0 : 180;
		FVector rowSize = FVector(size.X, parkingStripDepth, size.Z);
		// createParamsRow.offset = createParamsIn.offset + FVector(0, centerY, 0);
		rotation = FVector(0,0,rotationZ) + rotationBase;
		location = FVector(0, centerY, 0) + offsetBase;
		params.offset = location;
		params.rotation = rotation;
		if (rotationZ != 0) {
			// createParamsIn.rotateAround = MathVector::RotateVector(location, createParamsIn.rotation) + createParamsIn.offset;
			createParamsIn.rotateAround = MathVector::RotateVector(FVector(0,0,0), createParamsIn.rotation) + createParamsIn.offset;
		}
		LMParkingLot::CreateRow(rowSize, modelParams, createParamsIn, params);

		// for (int jj = 0; jj < treeCountInRow; jj++)
		// {
		// 	uName = Lodash::GetInstanceId("pineS");
		// 	pairsString = "mesh=pineS";
		// 	vertices = {MathVector::RotateVector(FVector(-(size.X / 2 - treeGap / 2) + jj * treeGap, -6, 0), createParamsRow.rotation) + createParamsRow.offset};
		// 	vertices = ModelBase::Vertices(vertices, createParamsIn);
		// 	polygons.Add(uName, FPolygon(uName, uName, vertices, FVector(0, 0, 0), "parkingLot", "point", pairsString));
		// }
		// centerY += parkingStripDepth + treeStripDepth + rowGap;

		// Reset.
		params.offset = offsetBase;
		params.rotation = rotationBase;
		createParamsIn.rotateAround = rotateAroundBase;
	}

	VerticesEdit *verticesEdit = VerticesEdit::GetInstance();
	verticesEdit->AddAndSave(polygons);
	return polygons;
}

TMap<FString, FPolygon> LMParkingLot::CreateRow(FVector size, FModelParams modelParams,
	FModelCreateParams createParamsIn, FParkingLot params) {
	FVector rotation = FVector(0,0,0), location = FVector(0,0,0), scale = FVector(1,1,1);
	FString uName, type, pairsString, scaleString, meshKey;
	TArray<FVector> vertices;
	TMap<FString, FPolygon> polygons = {};
	LoadContent* loadContent = LoadContent::GetInstance();

	auto [valid, meshesByTags] = loadContent->FillMeshesByTags(params.meshesByTags, { });
	if (!valid) {
		UE_LOG(LogTemp, Warning, TEXT("LMParkingLot.CreateRow missing meshesByTags, skipping"));
		return polygons;
	}
	params.meshesByTags = meshesByTags;

	float roadHeight = 0.25;
	uName = Lodash::GetInstanceId("ParkingRoad");
	// Give extra road on side to drive into parking spots
	scale = FVector(size.X, size.Y * 2, roadHeight);
	location = FVector(0,0,0) + params.offset;
	rotation = FVector(0,0,0) + params.rotation;
	pairsString = "mesh=cube&mat=smoothConcrete&scale=" + DataConvert::VectorToString(scale) +
		ModelBase::AddRotationString(createParamsIn.rotation, rotation);
	vertices = { MathVector::RotateVector(location, createParamsIn.rotation) + createParamsIn.offset };
	vertices = ModelBase::Vertices(vertices, createParamsIn, params.rotation);
	polygons.Add(uName, FPolygon(uName, uName, vertices, FVector(0, 0, 0), "parkingLot", "point", pairsString));

	float parameterMargin = 0;
	float solarChargingSectionLength = size.X - parameterMargin * 2;
	float solarChargingStationGap = 2;
	float singleStationLength = 20;
	int solarChargingStationCount = floor((solarChargingSectionLength + solarChargingStationGap) /
		(solarChargingStationGap + singleStationLength));

	float evChargerDepth = 1;
	float chargingStationStartX = -(size.X / 2 - parameterMargin - singleStationLength / 2);
	for (int ii = 0; ii < solarChargingStationCount; ii++)
	{
		uName = Lodash::GetInstanceId("SolarChargingStation");
		location = FVector(chargingStationStartX, 0, roadHeight) + params.offset;
		rotation = FVector(0,0,0) + params.rotation;
		pairsString = "mesh=solarChargingStation" + ModelBase::AddRotationString(createParamsIn.rotation, rotation);
		vertices = { MathVector::RotateVector(location, createParamsIn.rotation) + createParamsIn.offset };
		vertices = ModelBase::Vertices(vertices, createParamsIn, params.rotation);
		polygons.Add(uName, FPolygon(uName, uName, vertices, FVector(0, 0, 0), "parkingLot", "point", pairsString));

		float slot1X = chargingStationStartX - singleStationLength / 4;
		float slot2X = chargingStationStartX + singleStationLength / 4;
		uName = Lodash::GetInstanceId("EVCharger");
		location = FVector(slot1X, -(size.Y / 2) + 0.5, roadHeight) + params.offset;
		rotation = FVector(0,0,0) + params.rotation;
		pairsString = "mesh=evCharger" + ModelBase::AddRotationString(createParamsIn.rotation, rotation);
		vertices = { MathVector::RotateVector(location, createParamsIn.rotation) + createParamsIn.offset };
		vertices = ModelBase::Vertices(vertices, createParamsIn, params.rotation);
		polygons.Add(uName, FPolygon(uName, uName, vertices, FVector(0, 0, 0), "parkingLot", "point", pairsString));

		uName = Lodash::GetInstanceId("EVCharger");
		location = FVector(slot2X, -(size.Y / 2) + 0.5, roadHeight) + params.offset;
		rotation = FVector(0,0,0) + params.rotation;
		pairsString = "mesh=evCharger" + ModelBase::AddRotationString(createParamsIn.rotation, rotation);
		vertices = { MathVector::RotateVector(location, createParamsIn.rotation) + createParamsIn.offset };
		vertices = ModelBase::Vertices(vertices, createParamsIn, params.rotation);
		polygons.Add(uName, FPolygon(uName, uName, vertices, FVector(0, 0, 0), "parkingLot", "point", pairsString));

		float slot1CarProb = Lodash::RandomRangeFloat(0, 1);
		float slot2CarProb = Lodash::RandomRangeFloat(0, 1);

		if (slot1CarProb <= params.carProbability)
		{
			uName = Lodash::GetInstanceId("Car");
			location = FVector(slot1X, 0, roadHeight) + params.offset;
			rotation = FVector(0,0,0) + params.rotation;
			meshKey = params.meshesByTags["car"][Lodash::RandomRangeInt(0, params.meshesByTags["car"].Num() - 1)];
			pairsString = "mesh=" + meshKey + ModelBase::AddRotationString(createParamsIn.rotation, rotation);
			// note: this car mesh seems not centered in the model
			vertices = { MathVector::RotateVector(location, createParamsIn.rotation) + createParamsIn.offset };
			vertices = ModelBase::Vertices(vertices, createParamsIn, params.rotation);
			polygons.Add(uName, FPolygon(uName, uName, vertices, FVector(0, 0, 0), "parkingLot", "point", pairsString));
		}

		if (slot2CarProb <= params.carProbability)
		{
			uName = Lodash::GetInstanceId("Car");
			location = FVector(slot2X, 0, roadHeight) + params.offset;
			rotation = FVector(0,0,0) + params.rotation;
			meshKey = params.meshesByTags["car"][Lodash::RandomRangeInt(0, params.meshesByTags["car"].Num() - 1)];
			pairsString = "mesh=" + meshKey + ModelBase::AddRotationString(createParamsIn.rotation, rotation);
			vertices = { MathVector::RotateVector(location, createParamsIn.rotation) + createParamsIn.offset };
			vertices = ModelBase::Vertices(vertices, createParamsIn, params.rotation);
			polygons.Add(uName, FPolygon(uName, uName, vertices, FVector(0, 0, 0), "parkingLot", "point", pairsString));
		}

		chargingStationStartX += solarChargingStationGap + singleStationLength;
	}

	VerticesEdit *verticesEdit = VerticesEdit::GetInstance();
	verticesEdit->AddAndSave(polygons);
	return polygons;
}
