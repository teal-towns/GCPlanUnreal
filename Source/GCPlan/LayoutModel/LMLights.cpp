#include "LMLights.h"

#include "LayoutModelBase.h"
#include "../Common/DataConvert.h"
#include "../Common/Lodash.h"
#include "../Common/MathPolygon.h"
#include "../Common/MathVector.h"
#include "../Landscape/VerticesEdit.h"
#include "../Layout/LayoutPlace.h"
#include "../Layout/LayoutPolygon.h"
#include "../Mesh/LoadContent.h"
#include "../Modeling/ModelBase.h"
#include "../ModelingStructsActor.h"
#include "../BuildingStructsActor.h"

LMLights::LMLights() {
}

LMLights::~LMLights() {
}

TMap<FString, FPolygon> LMLights::Ceiling(FVector size, FModelParams modelParams,
	FModelCreateParams createParamsIn, FLayoutLights params) {
	FVector rotation = FVector(0,0,0), location = FVector(0,0,0), scale = FVector(1,1,1);
	FString uName, type, pairsString, scaleString, meshKey;
	TArray<FVector> vertices;
	TMap<FString, FPolygon> polygons = {};
	FString uNameBase = Lodash::GetInstanceId("lights");

	FPlaceParams placeParams;
	placeParams.skipMesh = true;
	placeParams.offsetAverage = params.placeOffset;
	placeParams.offsetMaxFactorX = params.placeOffsetMaxYFactor;
	placeParams.offsetMaxFactorY = params.placeOffsetMaxYFactor;
	vertices = MathPolygon::PointToBox(FVector(0,0,0), size + params.sizeBuffer);
	TMap<FString, FMeshTransform> transforms = LayoutPolygon::PlaceInPolygon(vertices, {}, placeParams);
	int counter = 0;
	float currentZ;
	for (auto& Elem : transforms) {
		FVector locationBase = Elem.Value.location;
		currentZ = size.Z;

		uName = uNameBase + "_" + FString::FromInt(counter) + "_light";
		scale = params.lightScale;
		location = FVector(locationBase.X, locationBase.Y, currentZ - scale.Z);
		pairsString = "meshRule=light&lightIntensity=" + FString::SanitizeFloat(params.lightIntensity) + "&scale=" + DataConvert::VectorToString(scale) +
			ModelBase::AddRotationString(createParamsIn.rotation);
		vertices = { MathVector::RotateVector(location + params.offset, createParamsIn.rotation) + createParamsIn.offset };
		vertices = ModelBase::Vertices(vertices, createParamsIn, params.rotation);
		polygons.Add(uName, FPolygon(uName, uName, vertices, FVector(0,0,0), "light", "point", pairsString));

		counter += 1;
	}

	VerticesEdit* verticesEdit = VerticesEdit::GetInstance();
	verticesEdit->AddAndSave(polygons);
	return polygons;
}
