#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "MeshTerrainActor.generated.h"

UCLASS()
class GCPLAN_API AMeshTerrainActor : public AActor
{
	GENERATED_BODY()
	
public:	
	AMeshTerrainActor();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(CallInEditor)
	void EditorInitMesh();
	UFUNCTION(CallInEditor)
	void EditorClearMesh();
	UPROPERTY(EditAnywhere)
	UMaterialInterface* Material;
	// UMaterialInstanceDynamic* Material;
	// UMaterialInstance* Material;
	void InitMesh();

private:
	UProceduralMeshComponent* ProceduralMesh;

};
