#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/StaticMeshActor.h"

// #include "CanvasTextWidget.h"

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
	void LisbonExteriors();
	UFUNCTION(BlueprintCallable)
	void LisbonWorld();
	UFUNCTION(BlueprintCallable)
	void LisbonWorldReScale();

	UFUNCTION(CallInEditor)
	void Test();

	// UPROPERTY(EditAnywhere)
	// UCanvasTextWidget* CanvasTextWidget;
	UPROPERTY(EditAnywhere)
	AStaticMeshActor* LineActorTemplate;

	// void CreateUI();

private:
	// void Init();
};
