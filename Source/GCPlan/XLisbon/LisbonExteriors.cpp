#include "LisbonExteriors.h"

#include "../BuildingStructsActor.h"
#include "../Common/DataConvert.h"
#include "../Common/Lodash.h"
#include "../Landscape/VerticesEdit.h"
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

TMap<FString, FPolygon> LisbonExteriors::ParkingLots() {
	FModelCreateParams createParamsIn;
	FVector rotation = FVector(0,0,0), location = FVector(0,0,0), scale = FVector(1,1,1);
	FString uName, type, pairsString, scaleString, meshKey;
	TArray<FVector> vertices;
	TMap<FString, FPolygon> polygons = {};

	FModelParams modelParams;
	FModelCreateParams createParams;

	float z = 7;

	// By batteries
	createParams.offset = FVector(44, -721, z);
	createParams.rotation = FVector(0, 0, 119);
	LMParkingLot::Create(FVector(325, 20, 0), modelParams, createParams);
	createParams.offset = FVector(30, -743, z);
	createParams.rotation = FVector(0, 0, 119);
	LMParkingLot::Create(FVector(325, 20, 0), modelParams, createParams);

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
	createParams.offset = FVector(-364, 272, z);
	createParams.rotation = FVector(0, 0, 28);
	LMParkingLot::Create(FVector(60, 16, 0), modelParams, createParams);

	// By train station
	createParams.offset = FVector(-336, -67, z);
	createParams.rotation = FVector(0, 0, 119);
	LMParkingLot::Create(FVector(230, 16, 0), modelParams, createParams);

	VerticesEdit* verticesEdit = VerticesEdit::GetInstance();
	verticesEdit->AddAndSave(polygons);
	return polygons;
}
