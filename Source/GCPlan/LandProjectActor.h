// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
// #include "GCPlanGameInstance.h"
#include "LandProjectActor.generated.h"

UCLASS()
class GCPLAN_API ALandProjectActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALandProjectActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void GetProject(FString);
	void GenerateLands(TArray<FLand>);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	bool Inited = false;
	// UGCPlanGameInstance* GameInstance;

	void Init();
};

USTRUCT()
struct FDataLogin {
	GENERATED_BODY()

	UPROPERTY()
	int valid;
	UPROPERTY()
	FString msg;
	UPROPERTY()
	TMap<FString, FString> user;
	UPROPERTY()
	TMap<FString, FString> user_preferences;
    // public UserCurrentStateClass user_current_state = new UserCurrentStateClass() {};
    // public List<ProjectClass> projects = new List<ProjectClass>() {};
    // public List<UserProjectClass> userProjects = new List<UserProjectClass>() {};

	// FDataLogin(int valid = 0, FString msg = "",  TMap<FString, FString> Auth = {}) {}
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
};

USTRUCT()
struct FLand {
	GENERATED_BODY()

	UPROPERTY()
	TMap<FString, FLandGameObject> game_objects;
	UPROPERTY()
    FString land_id = "";
};

USTRUCT()
struct FPlot {
	GENERATED_BODY()

	UPROPERTY()
	FString _id;
	UPROPERTY()
    FString uName;
    UPROPERTY()
	TMap<FString, float> posCenter;
	// TODO - need another struct? Can not do TMap inside TArray??
	// UPROPERTY()
    // TArray<TMap<FString, float>> vertices;
};

// TODO
// USTRUCT()
// struct FSupercell {
// 	GENERATED_BODY()

// 	UPROPERTY()
// 	public string _id;
// 	public string uName;
// 	public Dictionary<string, float> posCenterGround;
// 	public List<Dictionary<string, float>> vertices;
// 	public int minFloors;
// 	public int maxFloors;
// 	public int minFloorsSecondary;
// 	public int maxFloorsSecondary;
// 	public int ringWidthHexes;
// 	public float hexDiameter;
// 	public float floorHeight;
// 	public string category;
// 	public SuperCellStatsClass stats;
// 	public string land_id;
// };

USTRUCT()
struct FDataProject {
	GENERATED_BODY()

	UPROPERTY()
	int valid;
	UPROPERTY()
	FString msg;
	UPROPERTY()
	TArray<FLand> lands;

	// TODO
    // public ProjectClass project;
    // public List<PlotClass> plots;
    // public List<SupercellClass> supercells;
    // public List<PieceDBClass> pieces;
};
