#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InstancedStaticMeshActor.generated.h"

UCLASS()
class GCPLAN_API AInstancedStaticMeshActor : public AActor
{
	GENERATED_BODY()
	
public:	
	AInstancedStaticMeshActor();

	void CreateInstance(FVector, FRotator, FVector);
	// void SetMesh(FString);
	FString GetName();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere)
	UInstancedStaticMeshComponent* InstancedStaticMeshComponent;
};
