#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Landscape.h"

#include "BuildingStructsActor.h"
#include "DataStructsActor.h"

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
	void EditorGenerate();
	UFUNCTION(CallInEditor)
	void EditorClear();
	UPROPERTY(EditAnywhere)
	FEditorParamsLP EditorParams;
	UFUNCTION(CallInEditor)
	void EditorTakeAction();

	void Init();
	void InitSocketOn();
	void Login();

private:
	bool Inited = false;
};
