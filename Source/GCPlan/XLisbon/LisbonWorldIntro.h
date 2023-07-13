#pragma once

#include <mutex>
#include "Engine/StaticMeshActor.h"

// #include "../CanvasTextWidget.h"

class LisbonWorldIntro {
public:
	LisbonWorldIntro();
	~LisbonWorldIntro();

	// Singletons should not be cloneable or assignable.
	LisbonWorldIntro(LisbonWorldIntro &other) = delete;
	void operator=(const LisbonWorldIntro &) = delete;

	static LisbonWorldIntro *GetInstance();

	void Cables(UWorld* world, AStaticMeshActor* LineActorTemplate);
	// void ReScaleCables();
	// void UI(UCanvasTextWidget* CanvasTextWidget, int step = 0);
	TArray<FVector> SquigglePath(FVector start, FVector end, int numPieces = 2,
		int piecesPerCurve = 100, float sizeFactor = 0.5, float angleMin = 30, float angleMax = 60,
		float controlSizeMinFactor = 0.5, float controlSizeMaxFactor = 1);
	TArray<FVector> PointsToSquigglePath(TArray<FVector> verticesMain);

private:
	static LisbonWorldIntro *pinstance_;
	static std::mutex mutex_;

	// UWorld* _world;
	// FTimerHandle _uiTimer;
	// FTimerDelegate _uiDelegate;
};
