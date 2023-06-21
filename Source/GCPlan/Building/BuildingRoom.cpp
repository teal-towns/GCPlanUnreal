#include "BuildingRoom.h"

#include "../Common/Lodash.h"
#include "../Modeling/ModelBase.h"
#include "../Modeling/Common/ModelSide.h"

BuildingRoom::BuildingRoom() {
}

BuildingRoom::~BuildingRoom() {
}

void BuildingRoom::Build(TMap<FString, FString> pairs) {
	auto [key, modelParams] = ModelBase::ModelParamsFromPairs(pairs);
	auto [location, rotation, scale] = ModelBase::PairsToTransform(pairs);
	FModelCreateParams createParams;
	createParams.offset = location;
	createParams.rotation = rotation;
	if (!pairs.Contains("meshRule")) {
		pairs.Add("meshRule", "");
	}
	if (pairs["meshRule"] == "roomCube") {
		Cube(scale, modelParams, createParams, pairs);
	}
}

void BuildingRoom::Cube(FVector size, FModelParams modelParams,
	FModelCreateParams createParamsIn, TMap<FString, FString> pairs) {
	ModelBase* modelBase = ModelBase::GetInstance();
	FString name = Lodash::GetInstanceId("BuildRoom_");
	FVector rotation = FVector(0,0,0), location = FVector(0,0,0), scale = FVector(1,1,1);
	AActor* actor = modelBase->CreateActorEmpty(name, modelParams);
	modelParams.parent = actor->FindComponentByClass<USceneComponent>();

	modelParams.meshKey = "cube";
	modelParams.materialKey = "white";
	if (pairs.Contains("mat")) {
		modelParams.materialKey = pairs["mat"];
	}

	FVector scaleSide = FVector(0.01, 0.01, 0.01);
	FVector sizeSide = FVector(size.X, size.Y, size.Z);
	FVector offsetSide = FVector(0,0,0);
	FString materialKeyDefault = modelParams.materialKey;
	if (pairs.Contains("backMat")) {
		modelParams.materialKey = pairs["backMat"];
	} else {
		modelParams.materialKey = materialKeyDefault;
	}
	ModelSide::Back(name, sizeSide, scaleSide, modelParams, offsetSide);
	if (pairs.Contains("frontMat")) {
		modelParams.materialKey = pairs["frontMat"];
	} else {
		modelParams.materialKey = materialKeyDefault;
	}
	ModelSide::Front(name, sizeSide, scaleSide, modelParams, offsetSide);
	if (pairs.Contains("rightMat")) {
		modelParams.materialKey = pairs["rightMat"];
	} else {
		modelParams.materialKey = materialKeyDefault;
	}
	ModelSide::Right(name, sizeSide, scaleSide, modelParams, offsetSide);
	if (pairs.Contains("leftMat")) {
		modelParams.materialKey = pairs["leftMat"];
	} else {
		modelParams.materialKey = materialKeyDefault;
	}
	ModelSide::Left(name, sizeSide, scaleSide, modelParams, offsetSide);
	if (pairs.Contains("bottomMat")) {
		modelParams.materialKey = pairs["bottomMat"];
	} else {
		modelParams.materialKey = materialKeyDefault;
	}
	ModelSide::Bottom(name, size, scaleSide, modelParams, offsetSide);
	// TESTING
	// if (pairs.Contains("topMat")) {
	// 	modelParams.materialKey = pairs["topMat"];
	// } else {
	// 	modelParams.materialKey = materialKeyDefault;
	// }
	// ModelSide::Top(name, size, scaleSide, modelParams, offsetSide);

	ModelBase::SetTransformFromParams(actor, createParamsIn);
}
