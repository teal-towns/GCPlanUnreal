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
#include "../Mesh/LoadContent.h"
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

TMap<FString, FPolygon> LisbonExteriors::Trees(float zOffset) {
	FVector rotation = FVector(0,0,0), location = FVector(0,0,0), scale = FVector(1,1,1);
	FString uName, type, pairsString, scaleString, meshKey;
	TArray<FVector> vertices;
	TMap<FString, FPolygon> polygons = {};
	FString uNameBase = Lodash::GetInstanceId("Trees");

	LoadContent* loadContent = LoadContent::GetInstance();
	FString meshesTrees = Lodash::Join(loadContent->GetMeshNamesByTags({ "tree" }), ",");
	FString meshesBushes = Lodash::Join(loadContent->GetMeshNamesByTags({ "outdoorBush" }), ",");

	float z = zOffset + 1;
	// polygons
	TArray<TArray<FVector>> verticesGroup = {
		{ FVector(-435,225,z), FVector(-435,245,z), FVector(-416,243,z), FVector(-416,226,z) },
		{ FVector(-273,247,z), FVector(-293,278,z), FVector(-396,222,z), FVector(-357,149,z), FVector(-344,157,z), FVector(-366,197,z) },
		{ FVector(-356,71,z), FVector(-382,56,z), FVector(-447,183,z), FVector(-430,202,z) },
		{ FVector(-419,11,z), FVector(-432,11,z), FVector(-370,-77,z), FVector(-382,-86,z) },
		{ FVector(-355,-135,z), FVector(-344,-126,z), FVector(-310,-184,z), FVector(-323,-190,z) },
		{ FVector(-172,-267,z), FVector(-144,-310,z), FVector(-213,-350,z), FVector(-241,-299,z) }
	};

	for (int ii = 0; ii < verticesGroup.Num(); ii++) {
		uName = uNameBase + "_" + FString::FromInt(ii) + "_trees";
		vertices = verticesGroup[ii];
		pairsString = "meshes=" + meshesTrees + "&placeOffsetAverage=10&placeScaleMin=0.3&placeScaleMax=0.75";
		polygons.Add(uName, FPolygon(uName, uName, vertices, FVector(0, 0, 0), "tree", "polygon", pairsString));

		uName = uNameBase + "_" + FString::FromInt(ii) + "_bushes";
		vertices = verticesGroup[ii];
		pairsString = "meshes=" + meshesBushes + "&placeOffsetAverage=2";
		polygons.Add(uName, FPolygon(uName, uName, vertices, FVector(0, 0, 0), "outdoorBush", "polygon", pairsString));
	}

	// lines
	verticesGroup = {
		{ FVector(-511, 140, z), FVector(-445, 215, z) }
	};

	// TODO

	VerticesEdit* verticesEdit = VerticesEdit::GetInstance();
	verticesEdit->AddAndSave(polygons);
	return polygons;
}
