// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BuildingStructsActor.generated.h"

UCLASS()
class GCPLAN_API ABuildingStructsActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABuildingStructsActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};

USTRUCT()
struct FMapStringFloat {
	GENERATED_BODY()

	UPROPERTY()
	TMap<FString, float> v;

	FMapStringFloat() {};
	FMapStringFloat(TMap<FString, float> v_) {
		v = v_;
	}
};

USTRUCT()
struct FBuildingBlueprintJsonData {
	GENERATED_BODY()

	UPROPERTY()
	TArray<FMapStringFloat> openEdgePositions;
	UPROPERTY()
	int floor;
	UPROPERTY()
	FString edgeType;

	FBuildingBlueprintJsonData() {};
	FBuildingBlueprintJsonData(TArray<FMapStringFloat> openEdgePositions_, int floor_, FString edgeType_) {
		openEdgePositions = openEdgePositions_, floor = floor_, edgeType = edgeType_;
	}
};

struct FBuildData {
	FVector posCenterGround;
	FVector posCurrentGround;
	int unitsCreated = 0;
	int unitLengthsCreatedOriginRing = 0;
	TArray<FVector2D> filledCoords = {};

	FBuildData() {};
	FBuildData(FVector posCenterGround_, FVector posCurrentGround_) {
		posCenterGround = posCenterGround_, posCurrentGround = posCurrentGround_;
	};
};

struct FBuildingStats {
	int unitCount;
	int ringWidthUnits;
	int roofUnitCount = 0;
	float averageRadiusInner = 0;
	float averageRadiusOuter = 0;
	int residentCount = 0;
};

USTRUCT()
struct FLandGameObject {
	GENERATED_BODY()

	UPROPERTY()
	FString clone_name;
	UPROPERTY()
	TMap<FString, float> position;
	UPROPERTY()
	TMap<FString, float> rotation;
	UPROPERTY()
	TMap<FString, float> scale;
	UPROPERTY()
	TArray<FString> child_renames;
	UPROPERTY()
	FString jsonDataString = "";

	FLandGameObject() {};
	FLandGameObject(FString clone_name_, FVector position_, FRotator rotation_,
		FVector scale_, TArray<FString> child_renames_, FString jsonDataString_) {
		clone_name = clone_name_, child_renames = child_renames_, jsonDataString = jsonDataString_;
		// compile errors - can we use static functions in constructors, or no?
		// position = DataConvert::VectorToDict(position_);
		// rotation = DataConvert::RotatorToDict(rotation_);
		// scale = DataConvert::VectorToDict(scale_);
		position = TMap<FString, float> {
			{ "x", (float)position_.X },
			{ "y", (float)position_.Y },
			{ "z", (float)position_.Z },
		};
		rotation = TMap<FString, float> {
			{ "x", (float)rotation_.Roll },
			{ "y", (float)rotation_.Pitch },
			{ "z", (float)rotation_.Yaw },
		};
		scale = TMap<FString, float> {
			{ "x", (float)scale_.X },
			{ "y", (float)scale_.Y },
			{ "z", (float)scale_.Z },
		};
	}
};

USTRUCT()
struct FLand {
	GENERATED_BODY()

	UPROPERTY()
	TMap<FString, FLandGameObject> game_objects = {};
	UPROPERTY()
	FString land_id = "";
};

struct FBuildingBlueprint {
	int valid = 0;
	FString msg = "";
	FLand land;
	FBuildingStats stats;
	FVector posCenterGround = FVector(0,0,0);

	FBuildingBlueprint() {};
	FBuildingBlueprint(FLand land_, int valid_ = 0) {
		land = land_, valid = valid_;
	};
};

struct FPlot {
	FString _id;
	FString uName;
	// TMap<FString, float> posCenter;
	// TODO - need another struct? Can not do TMap inside TArray??
	// UPROPERTY()
	// TArray<TMap<FString, float>> vertices;
	FVector posCenter;
	TArray<FVector> vertices;
	FString buildPattern;
	float averagePlotDistance;
};

USTRUCT()
struct FRoadPath {
	GENERATED_BODY()

	UPROPERTY()
	FString uName;
	UPROPERTY()
	TArray<FVector> vertices;
	UPROPERTY()
	float widthMeters;
	UPROPERTY()
	FString type;
	UPROPERTY()
	float laneCount;

	FRoadPath() {};
	FRoadPath(FString uName_, TArray<FVector> vertices_, float widthMeters_, FString type_, float laneCount_ = 1) {
		uName = uName_;
		vertices = vertices_;
		widthMeters = widthMeters_;
		type = type_;
		laneCount = laneCount_;
	}
};
