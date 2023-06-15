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
	TOOLS,
	VEHICLE
};

UENUM()
enum ModelingSubCategoryAnimal {
};

UENUM()
enum ModelingSubCategoryArchitecture {
	ELEVATOR,
	FENCE,
	SOLAR_PANEL,
	STAIRS,
	STREETLIGHT,
	TRASH_CAN
};

UENUM()
enum ModelingSubCategoryClothes {
};

UENUM()
enum ModelingSubCategoryElectronics {
	CELL_PHONE,
	COMPUTER,
	KEYBOARD,
	LAPTOP,
	MONITOR,
	MOUSE,
	TV
};

UENUM()
enum ModelingSubCategoryFood {
	CUP,
	WATER_BOTTLE
};

UENUM()
enum ModelingSubCategoryFurniture {
	BED,
	BENCH,
	CABINET,
	CHAIR,
	COUCH,
	DESK,
	DOOR,
	LIGHT,
	MIRROR,
	PILLOW,
	PLANTER_BOX,
	TABLE,
	WINDOW
};

UENUM()
enum ModelingSubCategoryPlants {
};

UENUM()
enum ModelingSubCategorySports {
};

UENUM()
enum ModelingSubCategoryTools {
	PEN
};

UENUM()
enum ModelingSubCategoryVehicle {
	CAR,
	EVCHARGER,
	TRAIN
};

USTRUCT()
struct FModelingBase {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TEnumAsByte<ModelingCategory> category;

	// Conditional hide sub category
	UPROPERTY()
	FString subCategory;
	UPROPERTY(EditAnywhere, meta = (EditCondition = "category == ModelingCategory::ANIMAL", EditConditionHides))
	TEnumAsByte<ModelingSubCategoryAnimal> subCategoryAnimal;
	UPROPERTY(EditAnywhere, meta = (EditCondition = "category == ModelingCategory::ARCHITECTURE", EditConditionHides))
	TEnumAsByte<ModelingSubCategoryArchitecture> subCategoryArchitecture;
	UPROPERTY(EditAnywhere, meta = (EditCondition = "category == ModelingCategory::CLOTHES", EditConditionHides))
	TEnumAsByte<ModelingSubCategoryClothes> subCategoryClothes;
	UPROPERTY(EditAnywhere, meta = (EditCondition = "category == ModelingCategory::ELECTRONICS", EditConditionHides))
	TEnumAsByte<ModelingSubCategoryElectronics> subCategoryElectronics;
	UPROPERTY(EditAnywhere, meta = (EditCondition = "category == ModelingCategory::FOOD", EditConditionHides))
	TEnumAsByte<ModelingSubCategoryFood> subCategoryFood;
	UPROPERTY(EditAnywhere, meta = (EditCondition = "category == ModelingCategory::FURNITURE", EditConditionHides))
	TEnumAsByte<ModelingSubCategoryFurniture> subCategoryFurniture;
	UPROPERTY(EditAnywhere, meta = (EditCondition = "category == ModelingCategory::PLANTS", EditConditionHides))
	TEnumAsByte<ModelingSubCategoryPlants> subCategoryPlants;
	UPROPERTY(EditAnywhere, meta = (EditCondition = "category == ModelingCategory::SPORTS", EditConditionHides))
	TEnumAsByte<ModelingSubCategorySports> subCategorySports;
	UPROPERTY(EditAnywhere, meta = (EditCondition = "category == ModelingCategory::TOOLS", EditConditionHides))
	TEnumAsByte<ModelingSubCategoryTools> subCategoryTools;
	UPROPERTY(EditAnywhere, meta = (EditCondition = "category == ModelingCategory::VEHICLE", EditConditionHides))
	TEnumAsByte<ModelingSubCategoryVehicle> subCategoryVehicle;

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
	bool closeTop; // Closes the top face of the prism
	UPROPERTY(EditAnywhere)
	bool closeBottom; // Closes the bottom face of the prism
	UPROPERTY(EditAnywhere)
	FString tagsString;
	UPROPERTY()
	TArray<FString> tags;
};

USTRUCT()
struct FLayoutModelBaseParams {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	// homeRoom, officeRoom, homeBuilding, officeBuilding, parkingLot, transitBuilding
	FString category;
	UPROPERTY(EditAnywhere)
	// homeBuilding:
	// homeRoom: bathroom, bedroom, closet, diningRoom, familyRoom, gameRoom, kitchen
	// officeBuilding:
	// officeRoom: conferenceRoom, desksRoom, cafeteria, lobby, serverRoom
	// parkingLot: outdoorParking, parkingGarage
	// transitBuilding: trainStation
	FString subCategory;
	UPROPERTY()
	FString name;
	UPROPERTY(EditAnywhere)
	FVector size;
	// UPROPERTY(EditAnywhere)
	// FVector vertices;
	UPROPERTY(EditAnywhere)
	FString tagsString;
	UPROPERTY()
	TArray<FString> tags;
};
