#pragma once

#include <mutex>
#include "Engine/StaticMeshActor.h"

#include "../ModelingStructsActor.h"
#include "../Modeling/ModelBase.h"

class LayoutModelBase {
public:
	LayoutModelBase();
	~LayoutModelBase();

	// Singletons should not be cloneable or assignable.
	LayoutModelBase(LayoutModelBase &other) = delete;
	void operator=(const LayoutModelBase &) = delete;
	static LayoutModelBase *GetInstance();

	void SetWorld(UWorld*);
	UWorld* GetWorld();
	void Create();

	void SetInputs(FLayoutModelBaseParams);
	std::tuple<FLayoutModelBaseParams, FModelParams> GetInputs(FString defaultName, FVector defaultSize);
	void DestroyActors();
	void CleanUp();

private:
	static LayoutModelBase *pinstance_;
	static std::mutex mutex_;

	UWorld* World;
	FLayoutModelBaseParams _layoutParams;
};
