#include "LisbonWorldIntro.h"

#include "Engine/StaticMeshActor.h"

#include "../Common/Lodash.h"
#include "../Common/MathVector.h"
#include "../Common/UnrealGlobal.h"
#include "../Draw/DrawHighlight.h"
#include "../Landscape/VerticesEdit.h"
#include "../Modeling/ModelBase.h"
#include "../Move/MovePolyLine.h"

LisbonWorldIntro* LisbonWorldIntro::pinstance_{nullptr};
std::mutex LisbonWorldIntro::mutex_;

LisbonWorldIntro::LisbonWorldIntro() {
}

LisbonWorldIntro::~LisbonWorldIntro() {
}

LisbonWorldIntro *LisbonWorldIntro::GetInstance() {
	std::lock_guard<std::mutex> lock(mutex_);
	if (pinstance_ == nullptr) {
		pinstance_ = new LisbonWorldIntro();
	}
	return pinstance_;
}

void LisbonWorldIntro::Cables(UWorld* world, AStaticMeshActor* LineActorTemplate) {
	MovePolyLine* movePolyLine = MovePolyLine::GetInstance();
	movePolyLine->SetWorld(world);
	// If try to destroy actors, causes a crash the 2nd time play mode happens.
	movePolyLine->CleanUp();

	float zScale = 10000;
	float speed;
	float speedMin = 200000;
	float speedMax = 1000000;
	int startTickDelay;
	int startTickDelayMin = 0;
	int startTickDelayMax = 300;
	TArray<FString> colors = { "blueEmissive", "greenEmissive", "orangeEmissive", "purpleEmissive", "redEmissive", "yellowEmissive" };
	int colorsCount = colors.Num();
	// Default point (Lisbon data center) 38.988267, -8.965344
	float zGround = 0;
	TMap<FString, FVector> locations = {
		{ "barcelona", FVector(750000, -200000, zGround) },
		{ "bilbao", FVector(450000, -450000, zGround) },
		{ "brazil", FVector(-5000000, 4000000, -3000000) },
		{ "canary", FVector(-500000, 1200000, zGround) },
		{ "cairo", FVector(2000000, 0, zGround) },
		{ "italy", FVector(2000000, -2000000, zGround) },
		{ "lagos", FVector(1000000, 2000000, zGround) },
		{ "lisbon", FVector(-100, 100, zGround) },
		{ "london", FVector(500000, -1000000, zGround) },
		{ "newYork", FVector(-4800000, -1500000, -1800000) },
	};
	TArray<FMovePolyLine> cables = {
		// Canary Islands, Spain 28.0050668,-15.7003936 -> Sexial, Portugal 38.6368383,-9.2077008
		// FMovePolyline("canarySexial", { FVector(58227775 / 100, -122702638 / 100, zGround), FVector(2100120 / 100, -3904642 / 100, zGround) },
		// 	LineActorTemplate, Lodash::RandomRangeFloat(speedMin,speedMax), FVector(0,zScale,zScale),
		// 	Lodash::RandomRangeInt(startTickDelayMin, startTickDelayMax),
		// 	colors[Lodash::RandomRangeInt(0, colorsCount - 1)]),
		// { FVector(58227775 / 100, -122702638 / 100, zGround), FVector(2100120 / 100, -3904642 / 100, zGround) },
		// Canary Islands, Spain 28.0050668,-15.7003936 -> Carcavelos, Portugal

		FMovePolyLine("newYork1", { locations["newYork"], locations["lisbon"] },
			LineActorTemplate, 2000000, FVector(0,zScale,zScale), 0, "red"),
		FMovePolyLine("newYork2", { locations["newYork"] + FVector(-100000, 100000, 0), locations["bilbao"] },
			LineActorTemplate, 2000000, FVector(0,zScale,zScale), 6, "blue"),
		FMovePolyLine("brazil1", { locations["brazil"], locations["barcelona"] },
			LineActorTemplate, 2000000, FVector(0,zScale,zScale), 2, "green"),
		FMovePolyLine("brazil2", { locations["brazil"], locations["lisbon"] },
			LineActorTemplate, 2000000, FVector(0,zScale,zScale), 4, "orange"),
		FMovePolyLine("lagos1", { locations["lagos"], locations["lisbon"] },
			LineActorTemplate, 1000000, FVector(0,zScale,zScale), 10, "purple"),
		FMovePolyLine("lagos2", { locations["lagos"], locations["bilbao"] },
			LineActorTemplate, 1000000, FVector(0,zScale,zScale), 15, "yellow"),
		FMovePolyLine("canary1", { locations["canary"], locations["lisbon"] },
			LineActorTemplate, 500000, FVector(0,zScale,zScale), 50, "red"),
		FMovePolyLine("canary2", { locations["canary"], locations["bilbao"] },
			LineActorTemplate, 500000, FVector(0,zScale,zScale), 60, "blue"),
		FMovePolyLine("italy1", { locations["italy"], locations["bilbao"] },
			LineActorTemplate, 500000, FVector(0,zScale,zScale), 55, "green"),
		FMovePolyLine("italy2", { locations["italy"], locations["lisbon"] },
			LineActorTemplate, 500000, FVector(0,zScale,zScale), 65, "orange"),
		FMovePolyLine("cairo1", { locations["cairo"], locations["bilbao"] },
			LineActorTemplate, 500000, FVector(0,zScale,zScale), 70, "purple"),
		FMovePolyLine("cairo2", { locations["cairo"], locations["barcelona"] },
			LineActorTemplate, 500000, FVector(0,zScale,zScale), 80, "yellow"),
		FMovePolyLine("london1", { locations["london"], locations["bilbao"] },
			LineActorTemplate, 500000, FVector(0,zScale,zScale), 90, "red"),
		FMovePolyLine("london2", { locations["london"], locations["lisbon"] },
			LineActorTemplate, 500000, FVector(0,zScale,zScale), 100, "green"),

		// FMovePolyLine("test1", { FVector(-1000000, 1000000, zGround), FVector(-100, 100, zGround) },
		// 	LineActorTemplate, Lodash::RandomRangeFloat(speedMin,speedMax), FVector(0,zScale,zScale),
		// 	Lodash::RandomRangeInt(startTickDelayMin, startTickDelayMax),
		// 	"red"),
		// FMovePolyLine("test2", { FVector(1000000, 1000000, zGround), FVector(-100, 100, zGround) },
		// 	LineActorTemplate, Lodash::RandomRangeFloat(speedMin,speedMax), FVector(0,zScale,zScale),
		// 	Lodash::RandomRangeInt(startTickDelayMin, startTickDelayMax),
		// 	"blue"),
		// FMovePolyLine("test3", { FVector(1000000, -1000000, zGround), FVector(-100, 100, zGround) },
		// 	LineActorTemplate, Lodash::RandomRangeFloat(speedMin,speedMax), FVector(0,zScale,zScale),
		// 	Lodash::RandomRangeInt(startTickDelayMin, startTickDelayMax),
		// 	"green"),
		// FMovePolyLine("test4", { FVector(-1000000, -1000000, zGround), FVector(-100, 100, zGround) },
		// 	LineActorTemplate, Lodash::RandomRangeFloat(speedMin,speedMax), FVector(0,zScale,zScale),
		// 	Lodash::RandomRangeInt(startTickDelayMin, startTickDelayMax),
		// 	"orange")
	};
	FVector control, start, end;
	float zControl = 1000000;
	float len;
	int numPoints = 100;
	TArray<FVector> vertices;
	FString key;
	for (int cc = 0; cc < cables.Num(); cc++) {
		start = cables[cc].vertices[0];
		end = cables[cc].vertices[1];
		len = (end - start).Size();
		control = start + (end - start).GetClampedToMaxSize(len * 0.5);
		control.Z = zControl;
		vertices = MathVector::BeizerCurvePoints(start, end, control, numPoints);
		key = Lodash::GetInstanceId("cable_" + cables[cc].key);
		movePolyLine->Move(key, vertices, LineActorTemplate, cables[cc].materialKey,
			cables[cc].speed, cables[cc].scale, cables[cc].startTickDelay);

		// movePolyLine->CreateActors(key, vertices, LineActorTemplate, "red");
		// for (int ii = 0; ii < vertices.Num() - 1; ii++) {
		// 	FString actorName = movePolyLine->FormName(key, ii);
		// 	if (movePolyLine->_actors.Contains(actorName)) {
		// 		AActor* actor = movePolyLine->_actors[actorName];
		// 		float currentMaxScaleX = movePolyLine->GetLineMaxXScale(vertices[ii], vertices[ii + 1]);
		// 		FVector scale = FVector(currentMaxScaleX, zScale, zScale);
		// 		actor->SetActorScale3D(scale);
		// 	}
		// }
	}
}

void LisbonWorldIntro::ReScaleCables() {
	MovePolyLine* movePolyLine = MovePolyLine::GetInstance();
	movePolyLine->ReScale(10, 450);
}
