#include "LMWall.h"

#include "LayoutModelBase.h"
#include "../Common/DataConvert.h"
#include "../Common/Lodash.h"
#include "../Landscape/VerticesEdit.h"
#include "../Mesh/LoadContent.h"
#include "../Modeling/ModelBase.h"
#include "../Mesh/LoadContent.h"
#include "../ModelingStructsActor.h"
#include "../BuildingStructsActor.h"

LMWall::LMWall() {
}

LMWall::~LMWall() {
}

TMap<FString, FPolygon> LMWall::Wall(FVector size, FModelParams modelParams,
	FModelCreateParams createParamsIn, FLayoutWall params) {
	FVector rotation = FVector(0,0,0), location = FVector(0,0,0), scale = FVector(1,1,1);
	FString uName, type, pairsString, scaleString, meshKey;
	TArray<FVector> vertices;
	TMap<FString, FPolygon> polygons = {};
	LoadContent* loadContent = LoadContent::GetInstance();

	auto [valid, meshesByTags] = loadContent->FillMeshesByTags(params.meshesByTags, { "door" });
	if (!valid) {
		UE_LOG(LogTemp, Warning, TEXT("LMWall.Wall missing meshesByTags, skipping"));
		return polygons;
	}
	params.meshesByTags = meshesByTags;

	FVector rotateAroundBase = createParamsIn.rotateAround;

	float doorScaleY = 1;
	float doorTop = 2.3;
	float doorLeft = params.doorLeft;
	uName = Lodash::GetInstanceId("LMWall_");
	scale = size;
	location = params.offset;
	rotation = params.wallRotation;
	pairsString = "meshRule=roomWall&mat=" + params.materialKey + "&scale=" + DataConvert::VectorToString(scale) +
		ModelBase::AddRotationString(createParamsIn.rotation, rotation);
	pairsString += "&door1=1&door1YMin=" + Lodash::ToFixed(doorLeft) + "&door1XMax=" + Lodash::ToFixed(doorTop);
	vertices = { location + createParamsIn.offset };
	vertices = ModelBase::Vertices(vertices, createParamsIn);
	polygons.Add(uName, FPolygon(uName, uName, vertices, FVector(0,0,0), "wall", "point", pairsString));
	// Door
	uName = Lodash::GetInstanceId("LMWallDoor_");
	location = FVector(0, size.Y / -2 + doorLeft + doorScaleY / 2, 0) + params.offset;
	rotation = params.doorRotation;
	meshKey = params.meshesByTags["door"][Lodash::RandomRangeInt(0, params.meshesByTags["door"].Num() - 1)];
	pairsString = "mesh=" + meshKey + ModelBase::AddRotationString(createParamsIn.rotation, rotation);
	vertices = { location + createParamsIn.offset };
	createParamsIn.rotateAround = params.offset + createParamsIn.offset;
	vertices = ModelBase::Vertices(vertices, createParamsIn, params.rotation);
	createParamsIn.rotateAround = rotateAroundBase;
	polygons.Add(uName, FPolygon(uName, uName, vertices, FVector(0,0,0), "wallPart", "point", pairsString));

	VerticesEdit* verticesEdit = VerticesEdit::GetInstance();
	verticesEdit->AddAndSave(polygons);
	return polygons;
}
