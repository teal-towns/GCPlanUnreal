#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

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
	static void LisbonExteriors();

	// UPROPERTY(EditAnywhere)
	// UCanvasTextWidget* CanvasTextWidget;

	// void CreateUI();

private:
	// void Init();
};
