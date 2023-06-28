#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
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

private:
	// void Init();
};
