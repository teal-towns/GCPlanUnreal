#include "LMDesks.h"

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

LMDesks::LMDesks() {
}

LMDesks::~LMDesks() {
}

TMap<FString, FPolygon> LMDesks::TwoDesksRow(FVector size, FModelParams modelParams,
	FModelCreateParams createParamsIn, FDesks params) {
	FVector rotation = FVector(0,0,0), location = FVector(0,0,0), scale = FVector(1,1,1);
	FString uName, type, pairsString, scaleString, meshKey;
	TArray<FVector> vertices;
	TMap<FString, FPolygon> polygons = {};
	LoadContent* loadContent = LoadContent::GetInstance();
	FString uNameBase = Lodash::GetInstanceId("TwoDesksRow");

	auto [valid, meshesByTags] = loadContent->FillMeshesByTags(params.meshesByTags, { "officeChair", "indoorBush" });
	if (!valid) {
		UE_LOG(LogTemp, Warning, TEXT("LMDesks.TwoDesksRow missing meshesByTags, skipping"));
		return polygons;
	}
	params.meshesByTags = meshesByTags;

	// Build desks along Y axis.
	float desksYSize = size.Y - params.plantWidth * 2;
	int deskCountOneRow = round(desksYSize / params.deskWidth);
	float deskYSize = desksYSize / (float)deskCountOneRow;
	float rowXSize = size.X / 2;
	float desksYEnd = size.Y / 2 - params.plantWidth;
	float currentYStart = size.Y / -2;
	float currentX = size.X / -2 + rowXSize / 2;
	float desksYStart = currentYStart + params.plantWidth;
	FVector offsetBase = params.offset;
	FVector rotationBase = params.rotation;
	FVector rotateAroundBase = createParamsIn.rotateAround;
	float peopleProbabilityOriginal = params.peopleProbability;
	float rotationZ;
	float xTemp;
	FPlanterBox plantParams;
	FWallPlanterBox plantParamsBox;
	for (int xx = 0; xx < 2; xx++) {
		// Desks face each other, so rotate second set.
		rotationZ = xx == 0 ? 0 : 180;
		while (currentYStart < size.Y / 2) {
			if (currentYStart >= desksYEnd) {
				// scale = FVector(0.3, 2, 0.5);
				// plantParamsBox.offset = FVector(currentX - scale.X / 2, currentYStart, 0) + offsetBase;
				// LMRoomPlants::MultiPlantsBox(scale, modelParams, createParamsIn, plantParamsBox);
				xTemp = currentX + (rotationZ == 0 ? rowXSize / 4 : rowXSize / -4);
				scale = FVector(params.plantWidth, params.plantWidth, size.Z);
				plantParams.offset = FVector(xTemp, currentYStart + scale.Y / 2, 0) + offsetBase;
				plantParams.meshRule = "planterBox";
				plantParams.planterHeight = 0.3;
				LMRoomPlants::PlanterBox(scale, modelParams, createParamsIn, plantParams);
				currentYStart += params.plantWidth;
			} else if (currentYStart < desksYStart) {
				// Center at desks, so start more toward middle.
				xTemp = currentX + (rotationZ == 0 ? rowXSize / 4 : rowXSize / -4);
				for (int pp = 0; pp < params.plantsPerDeskRow; pp++) {
					scale = FVector(params.plantWidth, params.plantWidth, size.Z);
					plantParams.offset = FVector(xTemp, currentYStart + scale.Y / 2, 0) + offsetBase;
					plantParams.meshRule = "planterBoxCylinderRaised";
					plantParams.planterHeight = 1.3;
					LMRoomPlants::PlanterBox(scale, modelParams, createParamsIn, plantParams);
					// // plants
					// scale = FVector(params.plantWidth, params.plantWidth, params.planterHeight);
					// location = FVector(xTemp, currentYStart + scale.Y / 2, 0) + offsetBase;
					// uName = uNameBase + Lodash::GetInstanceId("_PlanterBox");
					// pairsString = "meshRule=planterBoxCylinderRaised&scale=" + DataConvert::VectorToString(scale) +
					// 	ModelBase::AddRotationString(createParamsIn.rotation);
					// vertices = { MathVector::RotateVector(location, createParamsIn.rotation) + createParamsIn.offset };
					// vertices = ModelBase::Vertices(vertices, createParamsIn, params.rotation);
					// polygons.Add(uName, FPolygon(uName, uName, vertices, FVector(0,0,0), "planterBox", "point", pairsString));
					// uName = uNameBase + Lodash::GetInstanceId("_Plant");
					// // scale = FVector(params.plantWidth, params.plantWidth, 1);
					// scale = FVector(1,1,1);
					// location.Z = params.planterHeight + offsetBase.Z;
					// meshKey = params.meshesByTags["indoorBush"][Lodash::RandomRangeInt(0, params.meshesByTags["indoorBush"].Num() - 1)];
					// pairsString = "mesh=" + meshKey + "&scale=" + DataConvert::VectorToString(scale) +
					// 	ModelBase::AddRotationString(createParamsIn.rotation);
					// vertices = { MathVector::RotateVector(location, createParamsIn.rotation) + createParamsIn.offset };
					// vertices = ModelBase::Vertices(vertices, createParamsIn, params.rotation);
					// polygons.Add(uName, FPolygon(uName, uName, vertices, FVector(0,0,0), "bush", "point", pairsString));

					xTemp += params.plantWidth * 1.5;
				}

				currentYStart += params.plantWidth;
			} else {
				scale = FVector(rowXSize, deskYSize, size.Z);
				location = FVector(currentX, currentYStart + scale.Y / 2, 0) + offsetBase;
				rotation = FVector(0,0,rotationZ) + rotationBase;
				// rotation = FVector(0,0,0) + rotationBase;
				params.offset = location;
				params.rotation = rotation;
				if (rotationZ != 0) {
					createParamsIn.rotateAround = MathVector::RotateVector(location, createParamsIn.rotation) + createParamsIn.offset;
				}
				// To prevent the same person twice, check here instead.
				if (Lodash::RandomRangeFloat(0,1) <= params.peopleProbability) {
					params.peopleProbability = 1;
					if (Lodash::RandomRangeInt(0, 1) == 1) {
						params.forceStandingDesk = 1;
					} else {
						params.forceSittingDesk = 1;
					}
				} else {
					params.peopleProbability = 0;
				}
				Desk(scale, modelParams, createParamsIn, params);
				currentYStart += deskYSize;
				// Reset.
				params.offset = offsetBase;
				params.rotation = rotationBase;
				createParamsIn.rotateAround = rotateAroundBase;

				params.peopleProbability = peopleProbabilityOriginal;
				if (params.forceSittingDesk) {
					params.peopleSittingIndex += 1;
				}
				if (params.forceStandingDesk) {
					params.peopleStandingIndex += 1;
				}
				params.forceSittingDesk = 0;
				params.forceStandingDesk = 0;
			}
		}
		currentX += rowXSize;
		// Reset
		currentYStart = size.Y / -2;
	}

	VerticesEdit* verticesEdit = VerticesEdit::GetInstance();
	verticesEdit->AddAndSave(polygons);
	return polygons;
}

TMap<FString, FPolygon> LMDesks::Desk(FVector size, FModelParams modelParams,
	FModelCreateParams createParamsIn, FDesks params) {
	FVector rotation = FVector(0,0,0), location = FVector(0,0,0), scale = FVector(1,1,1);
	FString uName, type, pairsString, scaleString, meshKey;
	TArray<FVector> vertices;
	TMap<FString, FPolygon> polygons = {};
	LoadContent* loadContent = LoadContent::GetInstance();

	auto [valid, meshesByTags] = loadContent->FillMeshesByTags(params.meshesByTags, { "officeChair" });
	if (!valid) {
		UE_LOG(LogTemp, Warning, TEXT("LMDesks.Desk missing meshesByTags, skipping"));
		return polygons;
	}
	params.meshesByTags = meshesByTags;

	float deskScaleX = size.X - params.chairWidth;
	bool standing = false;
	// Sitting vs standing.
	float deskHeight = params.deskHeightMin;
	if (!params.forceSittingDesk && (params.forceStandingDesk || Lodash::RandomRangeInt(0, 1) == 1)) {
		deskHeight = Lodash::RandomRangeFloat(params.deskHeightMax - 0.3, params.deskHeightMax);
		standing = true;
	}
	uName = Lodash::GetInstanceId("Desk");
	FString uNameBase = uName;
	// Desk in front, facing backward.
	scale = FVector(deskScaleX, size.Y, deskHeight);
	location = FVector(size.X / 2 - scale.X / 2, 0, 0) + params.offset;
	rotation = FVector(0,0,180) + params.rotation;
	FVector deskLocation = location;
	FVector deskScale = scale;
	FVector deskRotation = rotation;
	pairsString = "meshRule=desk&legs=1&scale=" + DataConvert::VectorToString(scale) +
		ModelBase::AddRotationString(createParamsIn.rotation, rotation);
	vertices = { MathVector::RotateVector(location, createParamsIn.rotation) + createParamsIn.offset };
	vertices = ModelBase::Vertices(vertices, createParamsIn, params.rotation);
	polygons.Add(uName, FPolygon(uName, uName, vertices, FVector(0,0,0), "desk", "point", pairsString));
	// Chair in back
	uName = uNameBase + "_Chair";
	location = FVector(size.X / -2 + params.chairWidth / 2, 0, 0) + params.offset;
	if (standing) {
		location += FVector(-0.3, 0, 0);
	}
	meshKey = params.meshesByTags["officeChair"][Lodash::RandomRangeInt(0, params.meshesByTags["officeChair"].Num() - 1)];
	pairsString = "mesh=" + meshKey + ModelBase::AddRotationString(createParamsIn.rotation, params.rotation, meshKey);
	vertices = { MathVector::RotateVector(location, createParamsIn.rotation) + createParamsIn.offset };
	vertices = ModelBase::Vertices(vertices, createParamsIn, params.rotation);
	polygons.Add(uName, FPolygon(uName, uName, vertices, FVector(0,0,0), "chair", "point", pairsString));

	bool havePerson = 0;
	if (Lodash::RandomRangeFloat(0,1) <= params.peopleProbability) {
		bool valid1 = false;
		if (deskHeight > params.deskHeightMax - 0.3 &&
			params.peopleStandingIndex < params.meshesByTags["peopleStanding"].Num()) {
			// Standing
			meshKey = params.meshesByTags["peopleStanding"][params.peopleStandingIndex];
			location += FVector(0.5, 0, 0);
			valid1 = 1;
		} else if (params.peopleSittingIndex < params.meshesByTags["peopleSitting"].Num()) {
			// Sitting
			meshKey = params.meshesByTags["peopleSitting"][params.peopleSittingIndex];
			location += FVector(0.2, 0, 0.02);
			valid1 = 1;
		}
		if (valid1) {
			havePerson = true;
			uName = uNameBase + "_Person";
			pairsString = "mesh=" + meshKey + ModelBase::AddRotationString(createParamsIn.rotation, params.rotation, meshKey);
			vertices = { MathVector::RotateVector(location, createParamsIn.rotation) + createParamsIn.offset };
			vertices = ModelBase::Vertices(vertices, createParamsIn, params.rotation);
			polygons.Add(uName, FPolygon(uName, uName, vertices, FVector(0,0,0), "person", "point", pairsString));
		}
	}

	// desk items
	if (params.meshesByTags["pcMonitor"].Num() > 0 && Lodash::RandomRangeFloat(0,1) <= params.deskItemsProbability) {
		uName = uNameBase + "_pcMonitor";
		location =  deskLocation + FVector(Lodash::RandomRangeFloat(0.5 * deskScale.X / 2, 0.8 * deskScale.X / 2),
			Lodash::RandomRangeFloat(-0.6 * deskScale.Y / 2, 0.6 * deskScale.Y / 2), deskLocation.Z + deskScale.Z);
		rotation = deskRotation;
		meshKey = params.meshesByTags["pcMonitor"][Lodash::RandomRangeInt(0, params.meshesByTags["pcMonitor"].Num() - 1)];
		pairsString = "mesh=" + meshKey + ModelBase::AddRotationString(createParamsIn.rotation, rotation, meshKey);
		vertices = { MathVector::RotateVector(location, createParamsIn.rotation) + createParamsIn.offset };
		vertices = ModelBase::Vertices(vertices, createParamsIn, params.rotation);
		polygons.Add(uName, FPolygon(uName, uName, vertices, FVector(0,0,0), "pcMonitor", "point", pairsString));
	}
	if (params.meshesByTags["laptop"].Num() > 0 && (havePerson || Lodash::RandomRangeFloat(0,1) <= params.deskItemsProbability)) {
		uName = uNameBase + "_laptop";
		location =  deskLocation + FVector(Lodash::RandomRangeFloat(-0.7 * deskScale.X / 2, 0 * deskScale.X / 2),
			Lodash::RandomRangeFloat(-0.7 * deskScale.Y / 2, 0.7 * deskScale.Y / 2), deskLocation.Z + deskScale.Z);
		rotation = deskRotation;
		meshKey = params.meshesByTags["laptop"][Lodash::RandomRangeInt(0, params.meshesByTags["laptop"].Num() - 1)];
		pairsString = "mesh=" + meshKey + ModelBase::AddRotationString(createParamsIn.rotation, rotation, meshKey);
		vertices = { MathVector::RotateVector(location, createParamsIn.rotation) + createParamsIn.offset };
		vertices = ModelBase::Vertices(vertices, createParamsIn, params.rotation);
		polygons.Add(uName, FPolygon(uName, uName, vertices, FVector(0,0,0), "laptop", "point", pairsString));
	}
	// Can add more too (keyboard, mouse, tablet, etc).

	VerticesEdit* verticesEdit = VerticesEdit::GetInstance();
	verticesEdit->AddAndSave(polygons);
	return polygons;
}
