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
#include "../Modeling/Building/ModelBuilding.h"
#include "../ProceduralModel/PMPlanePolygon.h"

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
	FVector topOffset = FVector(6.5, -5, 0);
	// Data center 1
	createParams.offset = FVector(-284,70,z) + topOffset;
	createParams.rotation = FVector(0, 0, -61);
	BuildingCondensers(FVector(82,40,1), modelParams, createParams);
	// Data center 2
	createParams.offset = FVector(-201,-79,z) + topOffset;
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

void LisbonExteriors::CreateBuildings() {
	FModelCreateParams createParams;
	FModelBuilding params;
	TArray<FVector> vertices = {
		FVector(-10,-10,0), FVector(-10,10,0), FVector(10,10,0), FVector(10,-10,0)
	};

	float z = 0;
	FVector offsetBase;
	float currentZ;
	// AActor* actor;

	FModelParams modelParams;
	FPlanePolygon planeParams;

	// L43
	offsetBase = FVector(50,0,0);
	currentZ = 0;
	params.materialKeyDefault = "OfficeWindows";
	// Convert to static mesh does not work well all together, so do 1. walls, 2. roofs, 3. grass separately.
	// params.doTop = false;
	// bottom
	params.materialsByVertex = {};
	// params.materialsByVertex.Add(1, "black");

	// vertices = { FVector(-18,32.5,z), FVector(-24,16,z), FVector(-33,21,z),
	// 	FVector(-33,-22,z), FVector(7,-21,z), FVector(5.5,2,z), FVector(8,8.5,z), FVector(16,4.5,z), FVector(22.5,18,z),
	// 	FVector(23.5,24.5,z), FVector(5,33,z) };
	// // Split into two for triangles on top.
	// currentZ = 1;
	// createParams.offset = offsetBase + FVector(0,0,currentZ);
	// params.height = 5;
	// params.triangleDirection = "counterClockwise";
	// ModelBuilding::Create(vertices, FModelParams(), createParams, params);

	// Split into two for triangles on top.
	vertices = { FVector(5.5,2,z), FVector(-24,16,z), FVector(-33,21,z), FVector(-33,-22,z), FVector(7,-21,z) };
	params.materialsByVertex = {};
	// Split into two for triangles on top.
	currentZ = 1;
	createParams.offset = offsetBase + FVector(0,0,currentZ);
	params.height = 5;
	params.triangleDirection = "counterClockwise";
	ModelBuilding::Create(vertices, FModelParams(), createParams, params);
	vertices = { FVector(-18,32.5,z), FVector(-24,16,z), FVector(5.5,2,z), FVector(8,8.5,z), FVector(16,4.5,z), FVector(22.5,18,z),
		FVector(23.5,24.5,z), FVector(5,33,z) };
	params.materialsByVertex = {};
	// currentZ = 1;
	createParams.offset = offsetBase + FVector(0,0,currentZ);
	params.height = 5;
	params.triangleDirection = "counterClockwise";
	ModelBuilding::Create(vertices, FModelParams(), createParams, params);

	// floor 1
	// vertices = { FVector(7,-22,z), FVector(-41,-21,z), FVector(-41,27,z), FVector(-34.5,24.5,z), FVector(-27,37,z),
	// 	FVector(24,36,z), FVector(24,19,z), FVector(17,5,z), FVector(10.5,7.5,z), FVector(7,2,z) };
	// currentZ += params.height;
	// createParams.offset = offsetBase + FVector(0,0,currentZ);
	// params.height = 13;
	// params.triangleDirection = "clockwise";
	// ModelBuilding::Create(vertices, FModelParams(), createParams, params);
	// Split into two for triangles on top.
	params.triangleDirection = "clockwise";
	vertices = { FVector(-34.5,24.5,z), FVector(-27,37,z),
		FVector(24,36,z), FVector(24,19,z), FVector(17,5,z), FVector(10.5,7.5,z), FVector(7,2,z) };
	params.materialsByVertex = { { 1, "OfficeWallWindows" }, { 3, "OfficeWallWindows" } };
	currentZ += params.height;
	createParams.offset = offsetBase + FVector(0,0,currentZ);
	params.height = 13;
	// params.topMaterial = "L43Top2";
	ModelBuilding::Create(vertices, FModelParams(), createParams, params);
	vertices = { FVector(7,-22,z), FVector(-41,-21,z), FVector(-41,27,z), FVector(-34.5,24.5,z), FVector(7,2,z) };
	params.materialsByVertex = { { 0, "OfficeWallWindows" }, { 1, "OfficeWallWindows" }, { 4, "OfficeWallWindows" } };
	// currentZ += params.height;
	createParams.offset = offsetBase + FVector(0,0,currentZ);
	params.height = 13;
	ModelBuilding::Create(vertices, FModelParams(), createParams, params);
	float level1Z = currentZ + params.height;

	// floor 2
	vertices = { FVector(-10.5,-15,z), FVector(-38,-1,z), FVector(-21,34,z), FVector(6.5,18.5,z) };
	params.materialsByVertex = {};
	currentZ += params.height;
	createParams.offset = offsetBase + FVector(0,0,currentZ);
	params.height = 5;
	// params.topMaterial = "L43Top";
	ModelBuilding::Create(vertices, FModelParams(), createParams, params);
	float topZ = currentZ + params.height;

	// grass top
	modelParams.materialKey = "OfficeGrass";

	createParams.offset = FVector(0,0,level1Z + 0.01);
	vertices = DataConvert::StringsToVectors({
		"187.1906,-46.2041,0.0763",
		"188.8149,-43.5480,0.0763",
		"188.8149,-43.5480,0.0763",
		"188.8149,-30.7989,0.0763",
		"156.5307,-31.0645,0.0763"
	}, 0);
	planeParams.name = "A";
	planeParams.triangleDirection = "counterClockwise";
	PMPlanePolygon::Create(vertices, createParams, modelParams, planeParams);
	vertices = DataConvert::StringsToVectors({
		"147.5967,-81.5297,0.0763",
		"132.7744,-74.0927,0.0763",
		"132.1652,-75.1552,0.0763",
		"132.3683,-81.2641,0.0763"
	}, 0);
	planeParams.name = "B";
	planeParams.triangleDirection = "counterClockwise";
	PMPlanePolygon::Create(vertices, createParams, modelParams, planeParams);
	vertices = DataConvert::StringsToVectors({
		"132.9774,-57.0939,0.0763",
		"138.8657,-45.4072,0.0763",
		"132.9774,-42.2200,0.0763"
	}, 0);
	planeParams.name = "C";
	planeParams.triangleDirection = "counterClockwise";
	PMPlanePolygon::Create(vertices, createParams, modelParams, planeParams);
	planeParams.triangleDirection = "clockwise";

	createParams.offset = FVector(0,0,topZ + 0.01);
	// planeParams.triangleDirection = "counterClockwise";
	vertices = DataConvert::StringsToVectors({
		"164.2464,-65.5933,0.0763",
		"170.1348,-54.1723,0.0763",
		"165.6677,-51.5162,0.0763",
		"159.5764,-63.4685,0.0763"
	}, 0);
	planeParams.name = "E";
	planeParams.triangleDirection = "counterClockwise";
	PMPlanePolygon::Create(vertices, createParams, modelParams, planeParams);
	vertices = DataConvert::StringsToVectors({
		"146.9876,-61.3436,0.0763",
		"159.9825,-67.9838,0.0763",
		"157.5459,-73.0303,0.0763",
		"137.2414,-63.2029,0.0763"
	}, 0);
	planeParams.name = "F";
	planeParams.triangleDirection = "clockwise";
	PMPlanePolygon::Create(vertices, createParams, modelParams, planeParams);
	vertices = DataConvert::StringsToVectors({
		"137.2414,-63.2029,0.0763",
		"146.9876,-42.7512,0.0763",
		"154.5002,-46.4697,0.0763",
		"146.9876,-61.3436,0.0763"
	}, 0);
	planeParams.name = "G";
	PMPlanePolygon::Create(vertices, createParams, modelParams, planeParams);


	// // L44
	// offsetBase = FVector(-100,0,0);
	// currentZ = 0;
	// params.materialKeyDefault = "OfficeWindows";

	// // bottom
	// // Split into two for triangles on top.
	// vertices = { FVector(44,-34,z), FVector(16.5,-44,z), FVector(-6,-43.5,z), FVector(-6,-22,z), FVector(-8.5,-15,z), FVector(28,-9.5,0), FVector(34.5,-7,z) };
	// params.materialsByVertex = {};
	// // Split into two for triangles on top.
	// currentZ = 1;
	// createParams.offset = offsetBase + FVector(0,0,currentZ);
	// params.height = 5;
	// ModelBuilding::Create(vertices, FModelParams(), createParams, params);
	// vertices = { FVector(28,-9.5,0), FVector(-8.5,-15,z), FVector(-14.5,-17,z), FVector(-20,-3.5,z), FVector(-19.5,11,z), FVector(20.5,11,z) };
	// params.materialsByVertex = {};
	// // currentZ = 1;
	// createParams.offset = offsetBase + FVector(0,0,currentZ);
	// params.height = 5;
	// ModelBuilding::Create(vertices, FModelParams(), createParams, params);

	// // floor 1
	// vertices = { FVector(-26,14,z), FVector(28,14,z), FVector(33,0,z), FVector(-9,-15.5,z), FVector(-16,-17,z) };
	// params.materialsByVertex = { { 0, "OfficeWallWindows" }, { 4, "OfficeWallWindows" } };
	// currentZ += params.height;
	// createParams.offset = offsetBase + FVector(0,0,currentZ);
	// params.height = 13;
	// ModelBuilding::Create(vertices, FModelParams(), createParams, params);
	// vertices = { FVector(-9,-15.5,z), FVector(33,0,z), FVector(52,6.5,z), FVector(51.5,-44,z), FVector(-6.5,-44.5,z), FVector(-7,-22.5,z) };
	// params.materialsByVertex = { { 2, "OfficeWallWindows" }, { 3, "OfficeWallWindows" } };
	// createParams.offset = offsetBase + FVector(0,0,currentZ);
	// actor = ModelBuilding::Create(vertices, FModelParams(), createParams, params);
	// float floor1Height = params.height;
	// AActor* floor1Actor = actor;

	// // top
	// vertices = DataConvert::StringsToVectors({
	// 	"27.1178,-8.8921,0.0763",
	// 	"34.8272,-7.4096,0.0763",
	// 	"44.6988,-34.8914,0.0763",
	// 	"5.9719,-34.8914,0.0763"
	// }, 0);
	// // vertices = { FVector(292,-32,z), FVector(288,-62.5,z), FVector(277,-78,z), FVector(264,-41.5,z) };
	// params.materialsByVertex = {};
	// currentZ += params.height;
	// createParams.offset = offsetBase + FVector(0,0,currentZ);
	// params.height = 5;
	// ModelBuilding::Create(vertices, FModelParams(), createParams, params);
	// vertices = DataConvert::StringsToVectors({
	// 	"27.1178,-8.8921,0.0763",
	// 	"5.9719,-34.8914,0.0763",
	// 	"-6.4704,0.9037,0.0763",
	// 	"20.8662,11.1680,0.0763"
	// }, 0);
	// // vertices = { FVector(277,-78,z), FVector(288,-62.5,z), FVector(306,-49.5,z), FVector(316,-43.5,z)};
	// params.materialsByVertex = {};
	// createParams.offset = offsetBase + FVector(0,0,currentZ);
	// params.height = 5;
	// actor = ModelBuilding::Create(vertices, FModelParams(), createParams, params);
	// AActor* topActor = actor;
	// float topHeight = params.height;

	// // grass top
	// modelParams.materialKey = "OfficeGrass";

	// actor = floor1Actor;
	// createParams.parentActor = actor;
	// modelParams.parent = actor->FindComponentByClass<USceneComponent>();
	// createParams.parent = modelParams.parent;
	// createParams.offset = FVector(0,0,floor1Height + 0.01);
	// vertices = DataConvert::StringsToVectors({
	// 	"45.0198,-37.8403,0.0763",
	// 	"46.3736,-41.8880,0.0763",
	// 	"14.4634,-42.1410,0.0763",
	// 	"12.9162,-37.5873,0.0763"
	// }, 0);
	// planeParams.name = "F";
	// PMPlanePolygon::Create(vertices, createParams, modelParams, planeParams);
	// vertices = DataConvert::StringsToVectors({
	// 	"48.6943,-36.3224,0.0763",
	// 	"48.6943,-36.3224,0.0763",
	// 	"35.9302,-1.4107,0.0763",
	// 	"48.6943,3.6489,0.0763"
	// }, 0);
	// planeParams.name = "E";
	// PMPlanePolygon::Create(vertices, createParams, modelParams, planeParams);
	// vertices = DataConvert::StringsToVectors({
	// 	"-17.3480,-5.7591,0.0763",
	// 	"-11.0806,-3.7095,0.0763",
	// 	"-12.4516,2.4394,0.0763",
	// 	"-23.2237,10.6380,0.0763"
	// }, 0);
	// planeParams.name = "B";
	// planeParams.triangleDirection = "counterClockwise";
	// PMPlanePolygon::Create(vertices, createParams, modelParams, planeParams);
	// vertices = DataConvert::StringsToVectors({
	// 	"-23.2237,10.6380,0.0763",
	// 	"-12.4516,2.4394,0.0763",
	// 	"4.6002,11.2384,0.0763",
	// 	"5.5672,8.9616,0.0763"
	// }, 0);
	// planeParams.name = "C";
	// PMPlanePolygon::Create(vertices, createParams, modelParams, planeParams);
	// planeParams.triangleDirection = "clockwise";

	// actor = topActor;
	// createParams.parentActor = actor;
	// modelParams.parent = actor->FindComponentByClass<USceneComponent>();
	// createParams.parent = modelParams.parent;
	// createParams.offset = FVector(0,0,topHeight + 0.01);
	// planeParams.triangleDirection = "counterClockwise";
	// vertices = DataConvert::StringsToVectors({
	// 	"39.9890,-30.7503,0.0763",
	// 	"32.8334,-10.5117,0.0763",
	// 	"25.4844,-12.5356,0.0763",
	// 	"23.1636,-31.0033,0.0763"
	// }, 0);
	// planeParams.name = "A";
	// PMPlanePolygon::Create(vertices, createParams, modelParams, planeParams);
	// vertices = DataConvert::StringsToVectors({
	// 	"18.8857,7.3073,0.0763",
	// 	"11.0514,4.2328,0.0763",
	// 	"23.1946,-30.6109,0.0763",
	// 	"23.1636,-31.0033,0.0763",
	// 	"25.4844,-12.5356,0.0763"
	// }, 0);
	// planeParams.name = "D";
	// PMPlanePolygon::Create(vertices, createParams, modelParams, planeParams);

	// // grass offset: -10.5,13.5,18.2
	// // roofs offset: -11.5,13.5,500
}
