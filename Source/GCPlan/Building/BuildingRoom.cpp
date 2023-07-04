#include "BuildingRoom.h"

#include "../Common/DataConvert.h"
#include "../Common/Lodash.h"
#include "../Modeling/ModelBase.h"
#include "../Modeling/Common/ModelSide.h"
#include "../ProceduralModel/PMWall.h"

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
	if (pairs["meshRule"] == "roomWall") {
		Wall(scale, modelParams, createParams, pairs);
	} else if (pairs["meshRule"] == "roomCube") {
		Cube(scale, modelParams, createParams, pairs);
	}
}

void BuildingRoom::Wall(FVector size, FModelParams modelParams,
	FModelCreateParams createParamsIn, TMap<FString, FString> pairs) {
	ModelBase* modelBase = ModelBase::GetInstance();
	FString name = Lodash::GetInstanceId("RoomWall_");
	FVector rotation = FVector(0,0,0), location = FVector(0,0,0), scale = FVector(1,1,1);
	AActor* actor = modelBase->CreateActorEmpty(name, modelParams);
	modelParams.parent = actor->FindComponentByClass<USceneComponent>();

	modelParams.materialKey = "white";
	if (pairs.Contains("mat")) {
		modelParams.materialKey = pairs["mat"];
	}

	FWall wallParams;
	float z = 0;
	float leftY, rightY, bottomX, topX;
	float holeYMin, holeYMax, holeXMin, holeXMax;
	float minX = 0;
	float minY = size.Y / -2;
	if (pairs.Contains("door1")) {
		holeYMin = pairs.Contains("door1YMin") ? DataConvert::Float(pairs["door1YMin"]) : 1;
		holeXMin = pairs.Contains("door1XMin") ? DataConvert::Float(pairs["door1XMin"]) : 0;
		holeYMax = pairs.Contains("door1YMax") ? DataConvert::Float(pairs["door1YMax"]) : holeYMin + 1;
		holeXMax = pairs.Contains("door1XMax") ? DataConvert::Float(pairs["door1XMax"]) : holeXMin + 2.5;
		leftY = minY + holeYMin;
		rightY = minY + holeYMax;
		bottomX = minX + holeXMin;
		topX = minX + holeXMax;
		wallParams.holesVertices.Add({
			FVector(bottomX, leftY, z), FVector(topX, leftY, z),
			FVector(topX, rightY, z), FVector(bottomX, rightY, z)
		});
	}
	if (pairs.Contains("window1")) {
		holeYMin = pairs.Contains("window1YMin") ? DataConvert::Float(pairs["window1YMin"]) : 3;
		holeXMin = pairs.Contains("window1XMin") ? DataConvert::Float(pairs["window1XMin"]) : 1;
		holeYMax = pairs.Contains("window1YMax") ? DataConvert::Float(pairs["window1YMax"]) : holeYMin + 1;
		holeXMax = pairs.Contains("window1XMax") ? DataConvert::Float(pairs["window1XMax"]) : holeXMin + 1.5;
		leftY = minY + holeYMin;
		rightY = minY + holeYMax;
		bottomX = minX + holeXMin;
		topX = minX + holeXMax;
		wallParams.holesVertices.Add({
			FVector(bottomX, leftY, z), FVector(topX, leftY, z),
			FVector(topX, rightY, z), FVector(bottomX, rightY, z)
		});
	}

	FModelCreateParams createParams;
	createParams.parentActor = actor;
	createParams.parent = modelParams.parent;
	PMWall::Create(size, createParams, modelParams, wallParams);
	// Create other side
	createParams.offset += FVector(0,0,-0.01);
	wallParams.triangleDirection = "clockwise";
	PMWall::Create(size, createParams, modelParams, wallParams);

	ModelBase::SetTransformFromParams(actor, createParamsIn);
}

void BuildingRoom::Cube(FVector size, FModelParams modelParams,
	FModelCreateParams createParamsIn, TMap<FString, FString> pairs) {
	ModelBase* modelBase = ModelBase::GetInstance();
	FString name = Lodash::GetInstanceId("Room_");
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
	if (!pairs.Contains("skipBack")) {
		if (pairs.Contains("backMat")) {
			modelParams.materialKey = pairs["backMat"];
		} else {
			modelParams.materialKey = materialKeyDefault;
		}
		ModelSide::Back(name, sizeSide, scaleSide, modelParams, offsetSide);
	}
	if (!pairs.Contains("skipFront")) {
		if (pairs.Contains("frontMat")) {
			modelParams.materialKey = pairs["frontMat"];
		} else {
			modelParams.materialKey = materialKeyDefault;
		}
		ModelSide::Front(name, sizeSide, scaleSide, modelParams, offsetSide);
	}
	if (!pairs.Contains("skipRight")) {
		if (pairs.Contains("rightMat")) {
			modelParams.materialKey = pairs["rightMat"];
		} else {
			modelParams.materialKey = materialKeyDefault;
		}
		ModelSide::Right(name, sizeSide, scaleSide, modelParams, offsetSide);
	}
	if (!pairs.Contains("skipLeft")) {
		if (pairs.Contains("leftMat")) {
			modelParams.materialKey = pairs["leftMat"];
		} else {
			modelParams.materialKey = materialKeyDefault;
		}
		ModelSide::Left(name, sizeSide, scaleSide, modelParams, offsetSide);
	}
	if (!pairs.Contains("skipBottom")) {
		if (pairs.Contains("bottomMat")) {
			modelParams.materialKey = pairs["bottomMat"];
		} else {
			modelParams.materialKey = materialKeyDefault;
		}
		ModelSide::Bottom(name, size, scaleSide, modelParams, offsetSide);
	}
	// TESTING
	// if (!pairs.Contains("skipTop")) {
	// 	if (pairs.Contains("topMat")) {
	// 		modelParams.materialKey = pairs["topMat"];
	// 	} else {
	// 		modelParams.materialKey = materialKeyDefault;
	// 	}
	// 	ModelSide::Top(name, size, scaleSide, modelParams, offsetSide);
	// }

	ModelBase::SetTransformFromParams(actor, createParamsIn);
}
