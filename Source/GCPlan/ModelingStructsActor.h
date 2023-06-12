#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ModelingStructsActor.generated.h"

UCLASS()
class GCPLAN_API AModelingStructsActor : public AActor
{
	GENERATED_BODY()
	
public:	
	AModelingStructsActor();

protected:
	virtual void BeginPlay() override;

public:	
	// virtual void Tick(float DeltaTime) override;

};

UENUM()
enum ModelingCategory {
	ANIMAL,
	ARCHITECTURE,
	CLOTHES,
	ELECTRONICS,
	FOOD,
	FURNITURE,
	PLANTS,
	SPORTS,
	VEHICLE
};

UENUM()
enum ModelingSubCategory
{
	BENCH,
	CHAIR,
	TABLE,
	STREETLIGHT,
	EVCHARGER
};

USTRUCT()
struct FModelingBase {
	GENERATED_BODY()

	// UPROPERTY(EditAnywhere)
	// TEnumAsByte<ModelingCategory> category;
	UPROPERTY(EditAnywhere)
	TEnumAsByte<ModelingSubCategory> subCategory;
	UPROPERTY(EditAnywhere)
	FString name;
	UPROPERTY(EditAnywhere)
	FVector size;
	UPROPERTY(EditAnywhere)
	FString tagsString;
	UPROPERTY()
	TArray<FString> tags;

	// FModelingBase() {};
};

UENUM()
enum ProceduralModelCategory {
	CUBE,
	CYLINDER,
	PRISM
};

USTRUCT()
struct FProceduralModelBase {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TEnumAsByte<ProceduralModelCategory> category;
	UPROPERTY(EditAnywhere)
	FString name;
	UPROPERTY(EditAnywhere)
	FVector size;
	UPROPERTY(EditAnywhere)
	FVector vertices;
	UPROPERTY(EditAnywhere)
	int32 sidesSegmentCount;
	UPROPERTY(EditAnywhere)
	float topOffsetWidth;
	UPROPERTY(EditAnywhere)
	bool capTop;
	UPROPERTY(EditAnywhere)
	bool capBottom;
	UPROPERTY(EditAnywhere)
	FString tagsString;
	UPROPERTY()
	TArray<FString> tags;
};
