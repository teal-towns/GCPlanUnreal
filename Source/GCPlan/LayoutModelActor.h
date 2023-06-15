#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "ModelingStructsActor.h"

#include "LayoutModelActor.generated.h"

UCLASS()
class GCPLAN_API ALayoutModelActor : public AActor
{
	GENERATED_BODY()
	
public:	
	ALayoutModelActor();

protected:
	virtual void BeginPlay() override;

public:	
	// virtual void Tick(float DeltaTime) override;

	UFUNCTION(CallInEditor)
	void GenerateModel();
	UFUNCTION(CallInEditor)
	void DestroyActors();
	UPROPERTY(EditAnywhere)
	FLayoutModelBaseParams LayoutParams;

	void Init();
};
