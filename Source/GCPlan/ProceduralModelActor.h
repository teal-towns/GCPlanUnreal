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

public:
	UFUNCTION(CallInEditor)
	void GenerateModel();
	UFUNCTION(CallInEditor)
	void DestroyActors();
	UPROPERTY(EditAnywhere)
	FProceduralModelBase ProceduralModelBase;

	void Init();
};
