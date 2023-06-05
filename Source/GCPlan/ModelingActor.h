#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "ModelingStructsActor.h"

#include "ModelingActor.generated.h"

UCLASS()
class GCPLAN_API AModelingActor : public AActor
{
	GENERATED_BODY()
	
public:	
	AModelingActor();

protected:
	virtual void BeginPlay() override;

public:	
	// virtual void Tick(float DeltaTime) override;

	UFUNCTION(CallInEditor)
	void GenerateModel();
	UPROPERTY(EditAnywhere)
	FModelingBase ModelingBase;

	void Init();
};
