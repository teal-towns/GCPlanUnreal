#include "LMOfficeDesks.h"

#include "../LayoutModelBase.h"
#include "../LMDesks.h"
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

LMOfficeDesks::LMOfficeDesks() {
}

LMOfficeDesks::~LMOfficeDesks() {
}

TMap<FString, FPolygon> LMOfficeDesks::Desks(FVector size, FModelParams modelParams,
	FModelCreateParams createParamsIn) {
	FVector rotation = FVector(0,0,0), location = FVector(0,0,0), scale = FVector(1,1,1);
	FString uName, type, pairsString, scaleString;
	TArray<FVector> vertices;
	TMap<FString, FPolygon> polygons = {};

	// uName = Lodash::GetInstanceId("Room");
	// pairsString = "meshRule=roomCube&mat=wood&bottomMat=carpetBlue&scale=" + DataConvert::VectorToString(size) +
	// 	ModelBase::AddRotationString(createParamsIn.rotation);
	// vertices = { createParamsIn.offset };
	// vertices = ModelBase::Vertices(vertices, createParamsIn);
	// polygons.Add(uName, FPolygon(uName, uName, vertices, FVector(0,0,0), "room", "point", pairsString));

	FDesks deskParams;
	// Rows of desks along left side of room.
	float buffer = 1;
	scale = FVector(4, (size.Y - buffer * 2) * 0.7, size.Z);
	float xStep = scale.X + buffer;
	float currentX = size.X / -2 + scale.X / 2 + buffer;
	float maxX = size.X / 2 - scale.X / 2 - buffer;
	// along left wall.
	float currentY = size.Y / -2 + buffer + scale.Y / 2;
	int counter = 0;
	while (currentX <= maxX) {
		deskParams.offset = FVector(currentX, currentY, 0);
		// Ensure no repeat people.
		if (counter == 0) {
			deskParams.meshesByTags["peopleSitting"] = { "manSittingLaptop2", "womanSittingPhone" };
			deskParams.meshesByTags["peopleStanding"] = { "manStandingSuit", "womanStandingCup", "manStandingCup" };
		} else if (counter == 1) {
			deskParams.meshesByTags["peopleSitting"] = { "womanSitting" };
			deskParams.meshesByTags["peopleStanding"] = { "manStandingPhone", "womanStanding", "womanStandingTablet" };
		} else {
			deskParams.meshesByTags["peopleSitting"] = {};
			deskParams.meshesByTags["peopleStanding"] = {};
		}
		LMDesks::TwoDesksRow(scale, modelParams, createParamsIn, deskParams);
		currentX += xStep;
		counter += 1;
	}

	VerticesEdit* verticesEdit = VerticesEdit::GetInstance();
	verticesEdit->AddAndSave(polygons);
	return polygons;
}
