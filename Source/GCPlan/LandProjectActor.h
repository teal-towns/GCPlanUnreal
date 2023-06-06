// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Landscape.h"
// #include "GCPlanGameInstance.h"

#include "BuildingStructsActor.h"
#include "SettingsActor.h"
#include "SocketActor.h"

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

	UFUNCTION(CallInEditor)
	void EditorGenerate();
	UFUNCTION(CallInEditor)
	void EditorClear();
	void Init();
	void InitSocketOn();
	void Login();

private:
	bool Inited = false;
	// UGCPlanGameInstance* GameInstance;

	ASettingsActor* SettingsActor;
	ASocketActor* SocketActor;
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

// TODO - figure out how to declar these ONCE (as either struct or USTRUCT in Building/BuildingStructs - get compile error now..)
// USTRUCT()
// struct FULandGameObject {
// 	GENERATED_BODY()

// 	UPROPERTY()
// 	FString clone_name;
// 	UPROPERTY()
// 	TMap<FString, float> position;
// 	UPROPERTY()
// 	TMap<FString, float> rotation;
// 	UPROPERTY()
// 	TMap<FString, float> scale;
// 	UPROPERTY()
// 	TArray<FString> child_renames;
// 	UPROPERTY()
// 	FString jsonDataString = "";
// };

// USTRUCT()
// struct FULand {
// 	GENERATED_BODY()

// 	UPROPERTY()
// 	TMap<FString, FULandGameObject> game_objects;
// 	UPROPERTY()
//     FString land_id = "";
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
