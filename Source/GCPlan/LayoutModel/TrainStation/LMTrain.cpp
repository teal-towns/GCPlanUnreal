#include "LMTrain.h"

#include "../LayoutModelBase.h"
#include "../../Common/DataConvert.h"
#include "../../Common/Lodash.h"
#include "../../Common/MathPolygon.h"
#include "../../Common/MathVector.h"
#include "../../Landscape/VerticesEdit.h"
#include "../../Layout/LayoutPlace.h"
#include "../../Layout/LayoutPolyLine.h"
#include "../../Mesh/LoadContent.h"
#include "../../Modeling/ModelBase.h"
#include "../../Mesh/LoadContent.h"
#include "../../ModelingStructsActor.h"
#include "../../BuildingStructsActor.h"

LMTrain::LMTrain() {
}

LMTrain::~LMTrain() {
}

TMap<FString, FPolygon> LMTrain::Create(FVector size, FModelParams modelParams,
	FModelCreateParams createParamsIn, FTrain params) {
	FVector rotation = FVector(0,0,0), location = FVector(0,0,0), scale = FVector(1,1,1);
	FString uName, type, pairsString, meshKey;
	TArray<FVector> vertices;
	TMap<FString, FPolygon> polygons = {};
	LoadContent* loadContent = LoadContent::GetInstance();
	FString uNameBase = Lodash::GetInstanceId("Train");

	meshKey = "train";
	FVector trainSize = loadContent->GetMeshSize(meshKey);
	FVector rotateAroundBase = createParamsIn.rotateAround;
	createParamsIn.rotateAround = MathVector::RotateVector(location, createParamsIn.rotation) + createParamsIn.offset;
	// Build from front and add trains to back.
	location.X = trainSize.X / -2;
	for (int ii = 0; ii < params.traincars; ii++) {
		uName = uNameBase + "_" + FString::FromInt(ii);
		// Need to animate these, so use actor instead of instanced mesh.
		pairsString = "mesh=" + meshKey + "&movable=1" + ModelBase::AddRotationString(createParamsIn.rotation);
		vertices = { location + createParamsIn.offset };
		vertices = ModelBase::Vertices(vertices, createParamsIn, createParamsIn.rotation);
		polygons.Add(uName, FPolygon(uName, uName, vertices, FVector(0,0,0), "train", "point", pairsString));
		location.X -= (trainSize.X + 0.3);
	}
	createParamsIn.rotateAround = rotateAroundBase;

	VerticesEdit* verticesEdit = VerticesEdit::GetInstance();
	verticesEdit->AddAndSave(polygons);
	return polygons;
}
