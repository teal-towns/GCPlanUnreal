#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "ModelingStructsActor.h"

#include "ProceduralModelActor.generated.h"

UCLASS()
class GCPLAN_API AProceduralModelActor : public AActor
{
	GENERATED_BODY()
	
public:	
	AProceduralModelActor();

protected:
	virtual void BeginPlay() override;

public:	
	// virtual void Tick(float DeltaTime) override;

	UFUNCTION(CallInEditor)
	void GenerateModel();
	UPROPERTY(EditAnywhere)
	FProceduralModelBase ProceduralModelBase;

	void Init();
};
