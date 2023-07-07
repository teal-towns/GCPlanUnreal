#include "LisbonExteriors.h"

#include "../BuildingStructsActor.h"
#include "../Common/DataConvert.h"
#include "../Common/Lodash.h"
#include "../Common/MathPolygon.h"
#include "../Common/MathVector.h"
#include "../Landscape/VerticesEdit.h"
#include "../Layout/LayoutPlace.h"
#include "../Layout/LayoutPolygon.h"
#include "../LayoutModel/LMParkingLot.h"
#include "../Modeling/ModelBase.h"

LisbonExteriors* LisbonExteriors::pinstance_{nullptr};
std::mutex LisbonExteriors::mutex_;

LisbonExteriors::LisbonExteriors() {
}

LisbonExteriors::~LisbonExteriors() {
}

LisbonExteriors *LisbonExteriors::GetInstance() {
	std::lock_guard<std::mutex> lock(mutex_);
	if (pinstance_ == nullptr) {
		pinstance_ = new LisbonExteriors();
	}
	return pinstance_;
}

void LisbonExteriors::Buildings(float zOffset) {
	FModelParams modelParams;
	FModelCreateParams createParams;
	float z = zOffset + 23;
	// Data center 1
	createParams.offset = FVector(-283,76,z);
	createParams.rotation = FVector(0, 0, -61);
	BuildingCondensers(FVector(82,40,1), modelParams, createParams);
	// Data center 2
	createParams.offset = FVector(-200,-75,z);
	createParams.rotation = FVector(0, 0, -61);
	BuildingCondensers(FVector(82,40,1), modelParams, createParams);
}

TMap<FString, FPolygon> LisbonExteriors::BuildingCondensers(FVector size, FModelParams modelParams,
	FModelCreateParams createParamsIn) {
	FVector rotation = FVector(0,0,0), location = FVector(0,0,0), scale = FVector(1,1,1);
	FString uName, type, pairsString, scaleString, meshKey;
	TArray<FVector> vertices;
	TMap<FString, FPolygon> polygons = {};

	FVector condenserGroupSize = FVector((1+1)*5, (1*3), 1);
	FPlaceParams placeParams;
	placeParams.skipMesh = true;
	placeParams.offsetX = condenserGroupSize.X + 5;
	placeParams.offsetY = condenserGroupSize.Y + 5;
	placeParams.offsetMaxFactorX = 0;
	placeParams.offsetMaxFactorY = 0;
	FVector sizeBuffer = FVector(10,10,0);
	vertices = MathPolygon::PointToBox(FVector(0,0,0), size - sizeBuffer);
	TMap<FString, FMeshTransform> transforms = LayoutPolygon::PlaceInPolygon(vertices, {}, placeParams);
	int counter = 0;
	FCondenser params;
	for (auto& Elem : transforms) {
		FVector locationBase = Elem.Value.location;
		params.offset = locationBase;
		CondenserGroup(condenserGroupSize, modelParams, createParamsIn, params);
	}

	VerticesEdit* verticesEdit = VerticesEdit::GetInstance();
	verticesEdit->AddAndSave(polygons);
	return polygons;
}

TMap<FString, FPolygon> LisbonExteriors::CondenserGroup(FVector size, FModelParams modelParams,
	FModelCreateParams createParamsIn, FCondenser params) {
	FVector rotation = FVector(0,0,0), location = FVector(0,0,0), scale = FVector(1,1,1);
	FString uName, type, pairsString, scaleString, meshKey;
	TArray<FVector> vertices;
	TMap<FString, FPolygon> polygons = {};
	FString uNameBase = Lodash::GetInstanceId("Condensers");

	FVector condenserSize = FVector(0.7, 1.1, 0.8);
	FVector spacing = FVector(1,0,0);
	float lengthCount = 3;
	float rowCount = 5;

	float currentX = size.X / -2;
	float currentY = size.Y / -2;
	float yStart = currentY;
	for (int ii = 0; ii < rowCount; ii++) {
		for (int jj = 0; jj < lengthCount; jj++) {
			uName = uNameBase + "_" + FString::FromInt(ii) + "_" + FString::FromInt(jj) + "_condenser";
			location = FVector(currentX, currentY, 0) + params.offset;
			rotation = FVector(0,0,0) + params.rotation;
			pairsString = "mesh=buildingCondenser" + ModelBase::AddRotationString(createParamsIn.rotation, rotation);
			vertices = { MathVector::RotateVector(location, createParamsIn.rotation) + createParamsIn.offset };
			vertices = ModelBase::Vertices(vertices, createParamsIn, params.rotation);
			polygons.Add(uName, FPolygon(uName, uName, vertices, FVector(0, 0, 0), "buildingPart", "point", pairsString));

			currentY += condenserSize.Y + spacing.Y;
		}
		currentX += condenserSize.X + spacing.X;
		currentY = yStart;
	}

	VerticesEdit* verticesEdit = VerticesEdit::GetInstance();
	verticesEdit->AddAndSave(polygons);
	return polygons;
}

TMap<FString, FPolygon> LisbonExteriors::ParkingLots(float zOffset) {
	FModelCreateParams createParamsIn;
	FVector rotation = FVector(0,0,0), location = FVector(0,0,0), scale = FVector(1,1,1);
	FString uName, type, pairsString, scaleString, meshKey;
	TArray<FVector> vertices;
	TMap<FString, FPolygon> polygons = {};

	FModelParams modelParams;
	FModelCreateParams createParams;

	float z = zOffset;

	// By batteries
	createParams.offset = FVector(59, -756, z);
	createParams.rotation = FVector(0, 0, 119);
	LMParkingLot::Create(FVector(270, 20, 0), modelParams, createParams);
	createParams.offset = FVector(46, -776, z);
	createParams.rotation = FVector(0, 0, 119);
	LMParkingLot::Create(FVector(270, 20, 0), modelParams, createParams);

	// By Data Center
	createParams.offset = FVector(-63, -248, z);
	createParams.rotation = FVector(0, 0, 28);
	LMParkingLot::Create(FVector(110, 16, 0), modelParams, createParams);
	createParams.offset = FVector(-71, -232, z);
	createParams.rotation = FVector(0, 0, 28);
	LMParkingLot::Create(FVector(110, 16, 0), modelParams, createParams);
	createParams.offset = FVector(-80, -217, z);
	createParams.rotation = FVector(0, 0, 28);
	LMParkingLot::Create(FVector(110, 16, 0), modelParams, createParams);

	// By generators
	createParams.offset = FVector(-364, 281, z);
	createParams.rotation = FVector(0, 0, 28);
	LMParkingLot::Create(FVector(60, 16, 0), modelParams, createParams);

	// By train station
	createParams.offset = FVector(-341, -70, z);
	createParams.rotation = FVector(0, 0, 119);
	LMParkingLot::Create(FVector(230, 16, 0), modelParams, createParams);

	VerticesEdit* verticesEdit = VerticesEdit::GetInstance();
	verticesEdit->AddAndSave(polygons);
	return polygons;
}
