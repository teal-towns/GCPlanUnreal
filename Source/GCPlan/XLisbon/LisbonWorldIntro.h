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
	void ReScaleCables();
	// void UI(UCanvasTextWidget* CanvasTextWidget, int step = 0);

private:
	static LisbonWorldIntro *pinstance_;
	static std::mutex mutex_;

	// UWorld* _world;
	// FTimerHandle _uiTimer;
	// FTimerDelegate _uiDelegate;
};
