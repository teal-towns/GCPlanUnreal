#include "ModelDesk.h"

#include "Engine/StaticMeshActor.h"

#include "../ModelBase.h"
#include "../Electronics/ModelComputer.h"
#include "../Electronics/ModelMonitor.h"
#include "../../Common/Lodash.h"
#include "../Common/ModelTile.h"

ModelDesk::ModelDesk() {
}

ModelDesk::~ModelDesk() {
}

void ModelDesk::Build(TMap<FString, FString> pairs) {
	auto [key, modelParams] = ModelBase::ModelParamsFromPairs(pairs);
	auto [location, rotation, scale] = ModelBase::PairsToTransform(pairs, FVector(0,0,0));
	FModelCreateParams createParams;
	createParams.offset = location;
	createParams.rotation = rotation;
	if (!pairs.Contains("meshRule")) {
		pairs.Add("meshRule", "");
	}
	if (pairs["meshRule"] == "desk") {
		Create(scale, modelParams, createParams, pairs);
	}
}

AActor* ModelDesk::CreateFromInputs() {
	ModelBase* modelBase = ModelBase::GetInstance();
	UWorld* World = modelBase->GetWorld();
	auto [modelingBase, modelParams] = modelBase->GetInputs("Desk", FVector(0.7,1.3,1));
	FString name = modelingBase.name;
	FVector size = modelingBase.size;
	TMap<FString, FString> pairs = modelingBase.pairs;
	return Create(size, modelParams, FModelCreateParams(), pairs);
}

AActor* ModelDesk::Create(FVector size, FModelParams modelParams,
	FModelCreateParams createParamsIn, TMap<FString, FString> pairs, FModelDesk params) {
	ModelBase* modelBase = ModelBase::GetInstance();
	FString name = Lodash::GetInstanceId("Desk_");
	FVector rotation = FVector(0,0,0), location = FVector(0,0,0), scale = FVector(1,1,1);
	AActor* actor = modelBase->CreateActorEmpty(name, modelParams);
	modelParams.parent = actor->FindComponentByClass<USceneComponent>();

	modelParams.meshKey = "cube";
	modelParams.materialKey = "wood";
	FActorSpawnParameters spawnParams;
	float thick = params.woodThickness;

	location = FVector(0, 0, size.Z - thick);
	scale = FVector(size.X, size.Y, thick);
	AStaticMeshActor* tileActor = modelBase->CreateActor(name + "_Top", location, rotation, scale, spawnParams, modelParams);

	if (pairs.Contains("tile")) {
		location.X += (size.X / -2.0);
		location.Y += (size.Y / -2.0);
		location.Z += (thick / -2.0);
		ModelTile::Top(tileActor, name + "_Top", location, rotation, scale, spawnParams, modelParams);
	}

	if (pairs.Contains("legs")) {
		modelParams.materialKey = "black";
		float legThick = thick;
		float offset = thick / 2.0;
		float legOff = 0.0;
		float legsHeight = size.Z - thick;
		float legZ = 0;
		// Make each leg half of legs height.
		FVector legSize = FVector(offset, offset, (legsHeight / 2.0));
		// Except if not tall enough, in which case bottom leg is fixed height.
		if (legsHeight < params.bottomLegMinHeight * 2) {
			legSize.Z = params.bottomLegMinHeight;
			if (legSize.Z > legsHeight) {
				legSize.Z = legsHeight;
			}
		}

		// Telescoping legs - bottom first, then if have extra height, the top.
		for (int ii = 0; ii < 2; ii++) {
			float offXY = (offset + (0.5 * legThick) + (0.5 * legOff));
			if (legSize.Z < 0.01) {
				break;
			}

			// Left Front Leg
			location = FVector(((size.X / 2.0) - offXY), ((size.Y / -2.0) + offXY), legZ);
			scale = FVector(legThick, legThick, legSize.Z);
			if (ii == 1) {
				modelBase->CreateActor(name + "_LegsLF1", location, rotation, scale, spawnParams, modelParams);
			} else {
				modelBase->CreateActor(name + "_LegsLF0", location, rotation, scale, spawnParams, modelParams);
			}

			// Left Back Leg
			location = FVector(((size.X / -2.0) + offXY), ((size.Y / -2.0) + offXY), legZ);
			scale = FVector(legThick, legThick, legSize.Z);
			if (ii == 1) {
				modelBase->CreateActor(name + "_LegsLB1", location, rotation, scale, spawnParams, modelParams);
			} else {
				modelBase->CreateActor(name + "_LegsLB0", location, rotation, scale, spawnParams, modelParams);
			}

			// Right Front Leg
			location = FVector(((size.X / 2.0) - offXY), ((size.Y / 2.0) - offXY), legZ);
			scale = FVector(legThick, legThick, legSize.Z);
			if (ii == 1) {
				modelBase->CreateActor(name + "_LegsRF1", location, rotation, scale, spawnParams, modelParams);
			} else {
				modelBase->CreateActor(name + "_LegsRF0", location, rotation, scale, spawnParams, modelParams);
			}

			// Right Back Leg
			location = FVector(((size.X / -2.0) + offXY), ((size.Y / 2.0) - offXY), legZ);
			scale = FVector(legThick, legThick, legSize.Z);
			if (ii == 1) {
				modelBase->CreateActor(name + "_LegsRB1", location, rotation, scale, spawnParams, modelParams);
			} else {
				modelBase->CreateActor(name + "_LegsRB0", location, rotation, scale, spawnParams, modelParams);
			}

			legThick /= 2.0;
			legOff += legThick;
			legZ += legSize.Z;
			legSize.Z = legsHeight - legSize.Z;
		}

	} else {
		modelParams.materialKey = "wood";
		// Right
		location = FVector(0, ((size.Y / -2.0) + (thick / 2.0)), 0);
		scale = FVector(size.X, thick, size.Z);
		AStaticMeshActor* tileActorR = modelBase->CreateActor(name + "_Right", location, rotation, scale, spawnParams, modelParams);

		if (pairs.Contains("tile")) {
			location.X += (size.X / -2.0);
			location.Y += (thick / -2.0);
			location.Z -= thick;
			ModelTile::Right(tileActorR, name + "_Right", location, rotation, scale, spawnParams, modelParams);
		}

		// Left
		location = FVector(0, ((size.Y / 2.0) - (thick / 2.0)), 0);
		scale = FVector(size.X, thick, size.Z);
		AStaticMeshActor* tileActorL = modelBase->CreateActor(name + "_Left", location, rotation, scale, spawnParams, modelParams);

		if (pairs.Contains("tile")) {
			location.X += (size.X / -2.0);
			location.Y += (thick / -2.0);
			location.Z -= thick;
			ModelTile::Left(tileActorL, name + "_Left", location, rotation, scale, spawnParams, modelParams);
		}

		// Back
		float gapBack = (0.4 * size.Z);
		location = FVector((size.X / -2.0), 0, gapBack);
		scale = FVector(thick, size.Y, (size.Z - gapBack));
		AStaticMeshActor* tileActorB = modelBase->CreateActor(name + "_Back", location, rotation, scale, spawnParams, modelParams);

		if (pairs.Contains("tile")) {
			location.X -= (thick / 2.0);
			location.Y += (size.Y / -2.0);
			location.Z -= thick;
			ModelTile::Back(tileActorB, name + "_Back", location, rotation, scale, spawnParams, modelParams);
		}

		// Front
		location = FVector((size.X / 2.0), 0, 0);
		scale = FVector(thick, size.Y, size.Z);
		AStaticMeshActor* tileActorF = modelBase->CreateActor(name + "_Front", location, rotation, scale, spawnParams, modelParams);

//		if (pairs.Contains("tile")) {
			location.X -= (thick / 2.0);
			location.Y += (size.Y / -2.0);
			location.Z -= thick;
			ModelTile::Front(tileActorF, name + "_Front", location, rotation, scale, spawnParams, modelParams);
//		}

		// Bottom
		location = FVector(0, 0, 0);
		scale = FVector(size.X, size.Y, thick);
		AStaticMeshActor* tileActorM = modelBase->CreateActor(name + "_Bottom", location, rotation, scale, spawnParams, modelParams);

//		if (pairs.Contains("tile")) {
			location.X += (size.X / -2.0);
			location.Y += (size.Y / -2.0);
			location.Z += (thick / -2.0);
			ModelTile::Bottom(tileActorM, name + "_Bottom", location, rotation, scale, spawnParams, modelParams);
//		}
	}

	FModelCreateParams createParams;
	if (pairs.Contains("keyboardMonitorMouse")) {
		AActor* actorTemp = modelBase->CreateActorEmpty(name + "Accessories", modelParams);
		createParams.parent = actorTemp->FindComponentByClass<USceneComponent>();
		createParams.parentActor = actorTemp;
		modelParams.parent = createParams.parent;
		float MonitorXoff = 0.00;
		float MonitorYoff = (-0.1 * size.Y);
		createParams.offset = FVector(MonitorXoff, MonitorYoff, (size.Z + thick));
		float MonitorXsiz = 0.05;
		float MonitorYsiz = 0.40;
		ModelMonitor::Create(FVector(MonitorXsiz, MonitorYsiz, 0.3), modelParams, createParams);
		float KeyboardXoff = (2.0 * (MonitorXoff + MonitorXsiz));
		float KeyboardYsiz = 0.2;
		float KeyboardYoff = ((MonitorYoff + MonitorYsiz) - (1.8 * KeyboardYsiz));
		createParams.offset = FVector(KeyboardXoff, KeyboardYoff, (size.Z + thick));
		float KeyboardZsiz = 0.02;
		ModelComputer::Keyboard(FVector(0.1, KeyboardYsiz, KeyboardZsiz), modelParams, createParams);
		float MouseYoff = (KeyboardYoff - (1.0 * KeyboardYsiz));
		createParams.offset = FVector(KeyboardXoff, MouseYoff, (size.Z + thick));
		ModelComputer::Mouse(FVector(0.05, 0.03, KeyboardZsiz), modelParams, createParams);
	}

	ModelBase::SetTransformFromParams(actor, createParamsIn);
	return actor;
}
