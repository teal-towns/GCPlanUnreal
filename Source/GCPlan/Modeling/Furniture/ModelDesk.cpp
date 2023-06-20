#include "ModelDesk.h"

#include "Engine/StaticMeshActor.h"

#include "../ModelBase.h"
#include "../Electronics/ModelComputer.h"
#include "../Electronics/ModelMonitor.h"
#include "../../Common/Lodash.h"

ModelDesk::ModelDesk() {
}

ModelDesk::~ModelDesk() {
}

AActor* ModelDesk::CreateFromInputs() {
	ModelBase* modelBase = ModelBase::GetInstance();
	UWorld* World = modelBase->GetWorld();
	auto [modelingBase, modelParams] = modelBase->GetInputs("Desk", FVector(0.7,1.3,1));
	FString name = modelingBase.name;
	FVector size = modelingBase.size;
	TArray<FString> tags = modelingBase.tags;
	return Create(size, modelParams, FModelCreateParams(), tags);
}

AActor* ModelDesk::Create(FVector size, FModelParams modelParams,
	FModelCreateParams createParamsIn, TArray<FString> tags, TMap<FString, float> dimensions) {
	ModelBase* modelBase = ModelBase::GetInstance();
	FString name = Lodash::GetInstanceId("Desk_");
	FVector rotation = FVector(0,0,0), location = FVector(0,0,0), scale = FVector(1,1,1);
	AActor* actor = modelBase->CreateActorEmpty(name, modelParams);
	modelParams.parent = actor->FindComponentByClass<USceneComponent>();

	modelParams.meshKey = "cube";
	modelParams.materialKey = "wood";
	FActorSpawnParameters spawnParams;
	if (!dimensions.Contains("woodThickness")) {
		dimensions.Add("woodThickness", 0.05);
	}
	float thick = dimensions["woodThickness"];

	location = FVector(0, 0, size.Z);
	scale = FVector(size.X, size.Y, thick);
	modelBase->CreateActor(name + "_Top", location, rotation, scale, spawnParams, modelParams);

	if (tags.Contains("legs")) {
		modelParams.materialKey = "black";
		float legThick = thick;
		float offset = thick / 2.0;
		float legOff = 0.0;
		FVector legSize = FVector(offset, offset, (size.Z / 2.0));

		for (int ii = 0; ii < 2; ii++) {
			float offXY = (offset + (0.5 * legThick) + (0.5 * legOff));
			float legZ = legSize.Z * (float)ii;

			// Left Front Leg
			location = FVector(size.X / 2 - offXY, size.Y / -2 + offXY, legZ);
			scale = FVector(legThick, legThick, legSize.Z);
			if (ii == 1) {
				modelBase->CreateActor(name + "_LegsLF1", location, rotation, scale, spawnParams, modelParams);
			} else {
				modelBase->CreateActor(name + "_LegsLF0", location, rotation, scale, spawnParams, modelParams);
			}

			// Left Back Leg
			location = FVector(size.X / -2 + offXY, size.Y / -2 + offXY, legZ);
			scale = FVector(legThick, legThick, legSize.Z);
			if (ii == 1) {
				modelBase->CreateActor(name + "_LegsLB1", location, rotation, scale, spawnParams, modelParams);
			} else {
				modelBase->CreateActor(name + "_LegsLB0", location, rotation, scale, spawnParams, modelParams);
			}

			// Right Front Leg
			location = FVector(size.X / 2 - offXY, size.Y / 2 - offXY, legZ);
			scale = FVector(legThick, legThick, legSize.Z);
			if (ii == 1) {
				modelBase->CreateActor(name + "_LegsRF1", location, rotation, scale, spawnParams, modelParams);
			} else {
				modelBase->CreateActor(name + "_LegsRF0", location, rotation, scale, spawnParams, modelParams);
			}

			// Right Back Leg
			location = FVector(size.X / -2 + offXY, size.Y / 2 - offXY, legZ);
			scale = FVector(legThick, legThick, legSize.Z);
			if (ii == 1) {
				modelBase->CreateActor(name + "_LegsRB1", location, rotation, scale, spawnParams, modelParams);
			} else {
				modelBase->CreateActor(name + "_LegsRB0", location, rotation, scale, spawnParams, modelParams);
			}

			legThick /= 2.0;
			legOff += legThick;
		}

	} else {
		modelParams.materialKey = "wood";
		// Left
		location = FVector(0, size.Y / -2 + thick / 2, 0);
		scale = FVector(size.X, thick, size.Z);
		modelBase->CreateActor(name + "_Left", location, rotation, scale, spawnParams, modelParams);

		// Right
		location = FVector(0, size.Y / 2 - thick / 2, 0);
		scale = FVector(size.X, thick, size.Z);
		modelBase->CreateActor(name + "_Right", location, rotation, scale, spawnParams, modelParams);

		// Back
		float gapBack = (0.4 * size.Z);
		location = FVector(size.X / -2 + thick / 2, 0, gapBack);
		scale = FVector(thick, (size.Y - (2.0 * thick)), (size.Z - gapBack));
		modelBase->CreateActor(name + "_Back", location, rotation, scale, spawnParams, modelParams);
	}

	FModelCreateParams createParams;
	if (tags.Contains("keyboardMonitorMouse")) {
		AActor* actorTemp = modelBase->CreateActorEmpty(name + "Accessories", modelParams);
		createParams.parent = actorTemp->FindComponentByClass<USceneComponent>();
		createParams.parentActor = actorTemp;
		modelParams.parent = createParams.parent;
		createParams.offset = FVector(0, (-0.25 * size.Y), (size.Z + thick));
		ModelMonitor::Create(FVector(0.05, 0.4, 0.3), modelParams, createParams);
		createParams.offset = FVector((-0.10 * size.X), 0, (size.Z + thick));
		ModelComputer::Keyboard(FVector(0.2, 0.1, 0.02), modelParams, createParams);
		createParams.offset = FVector((0.10 * size.X), 0, (size.Z + thick));
		ModelComputer::Mouse(FVector(0.05, 0.02, 0.02), modelParams, createParams);
	}

	ModelBase::SetTransformFromParams(actor, createParamsIn);
	return actor;
}
