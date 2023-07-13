#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/StaticMeshActor.h"

#include "CanvasTextWidget.h"

#include "GlobalActor.generated.h"

UCLASS()
class GCPLAN_API AGlobalActor : public AActor
{
	GENERATED_BODY()
	
public:	
	AGlobalActor();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void LisbonWorld();
	// UFUNCTION(BlueprintCallable)
	// void LisbonWorldReScale();
	UFUNCTION()
	// void LisbonWorldUI(UCanvasTextWidget* CanvasTextWidget1, int step = 0);
	void LisbonWorldUI(int step = 0);
	UFUNCTION()
	void LisbonWorldText3D(int step = 0);

	UFUNCTION(BlueprintCallable)
	void LisbonInteriorsStart();
	UFUNCTION()
	void LisbonInteriors(int step = 0);

	UFUNCTION(BlueprintCallable)
	void LisbonExteriorsTrainStart();
	UFUNCTION()
	void LisbonExteriorsTrain(int step = 0);

	UFUNCTION(BlueprintCallable)
	void LisbonExteriorsTwoStart();
	UFUNCTION()
	void LisbonExteriorsTwo(int step = 0);

	UFUNCTION(CallInEditor)
	void Test();

	UPROPERTY(EditAnywhere)
	UCanvasTextWidget* CanvasTextWidget;
	UPROPERTY(EditAnywhere)
	AStaticMeshActor* LineActorTemplate;

	// void CreateUI();

private:
	// void Init();
	UWorld* _world;

	FTimerHandle _uiTimer;
	FTimerDelegate _uiDelegate;
	// int _step = 0;
	FTimerHandle _text3DTimer;
	FTimerDelegate _text3DDelegate;

	FTimerHandle _interiorsTimer;
	FTimerDelegate _interiorsDelegate;

	FTimerHandle _exteriorsTrainTimer;
	FTimerDelegate _exteriorsTrainDelegate;

	FTimerHandle _exteriorsTwoTimer;
	FTimerDelegate _exteriorsTwoDelegate;
};
