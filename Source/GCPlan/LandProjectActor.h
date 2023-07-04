#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Landscape.h"
#include "LevelSequence.h"
#include "LevelSequenceActor.h"

#include "BuildingStructsActor.h"
#include "DataStructsActor.h"
#include "CanvasTextWidget.h"

#include "LandProjectActor.generated.h"

UENUM()
enum EditorActionsLP {
	PLOTSREMOVECHILDREN
};

USTRUCT()
struct FEditorParamsLP {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TEnumAsByte<EditorActionsLP> Action;
};

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
	void Clear();
	UPROPERTY(EditAnywhere)
	FEditorParamsLP EditorParams;
	UFUNCTION(CallInEditor)
	void TakeAction();
	UFUNCTION(CallInEditor)
	void DrawVertices();
	UFUNCTION(CallInEditor)
	void SetVertices();
	UFUNCTION(CallInEditor)
	void Test();
	// UFUNCTION(CallInEditor)
	// void InitSequencer();

	UPROPERTY(EditAnywhere)
	UCanvasTextWidget* CanvasTextWidget;
	UPROPERTY(EditAnywhere)
	ALevelSequenceActor* LisbonSequence;

	void Init();
	void InitSocketOn();
	void Login();

	void CreateUI();

private:
	bool Inited = false;
};
