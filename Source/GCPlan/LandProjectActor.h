#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Landscape.h"

#include "BuildingStructsActor.h"
#include "DataStructsActor.h"
#include "Data/DataFileProject.h"

#include "LandProjectActor.generated.h"

UCLASS()
class GCPLAN_API ALandProjectActor : public AActor
{
	GENERATED_BODY()
	
public:	
	ALandProjectActor();

protected:
	virtual void BeginPlay() override;

	void GetProject(FString);

public:	
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(CallInEditor)
	void EditorGenerate();
	UFUNCTION(CallInEditor)
	void EditorClear();
	void Init();
	void InitSocketOn();
	void Login();

private:
	bool Inited = false;
};
