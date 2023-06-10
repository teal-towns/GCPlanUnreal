#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "BuildingStructsActor.h"

#include "DataStructsActor.generated.h"

UCLASS()
class GCPLAN_API ADataStructsActor : public AActor
{
	GENERATED_BODY()
	
public:	
	ADataStructsActor();

protected:
	virtual void BeginPlay() override;

public:	
	// virtual void Tick(float DeltaTime) override;

	static FDataProjectJsonSimplified ProjectToSimplified(FDataProjectJson data);
	static FDataProjectJson ProjectFromSimplified(FDataProjectJsonSimplified dataSimplified);

};

USTRUCT()
struct FDataSettings {
	GENERATED_BODY()

	UPROPERTY()
	FString loginEmail;
	UPROPERTY()
	FString loginPassword;
	UPROPERTY()
	FString projectUName;
	UPROPERTY()
	TMap<FString, FString> projectJsonFiles;
	UPROPERTY()
	FString urlWebsocket;
	UPROPERTY()
	FString contentMeshesJsonFile;
	UPROPERTY()
	int performanceQuantityLevel;
	UPROPERTY()
	int performanceQualityLevel;

	FDataSettings() {};
	FDataSettings(FString loginEmail_, FString loginPassword_, FString projectUName_,
		TMap<FString, FString> projectJsonFiles_, FString urlWebsocket_) {
		loginEmail = loginEmail_;
		loginPassword = loginPassword_;
		projectUName = projectUName_;
		projectJsonFiles = projectJsonFiles_;
		urlWebsocket = urlWebsocket_;
	};
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

USTRUCT()
struct FDataProjectJsonSimplified {
	GENERATED_BODY()

	UPROPERTY()
	TMap<FString, FPlotSimplified> plots;

	FDataProjectJsonSimplified() {};
	FDataProjectJsonSimplified(TMap<FString, FPlotSimplified> plots_) {
		plots = plots_;
	};
};

USTRUCT()
struct FDataProjectJson {
	GENERATED_BODY()

	UPROPERTY()
	TMap<FString, FPlot> plots;

	FDataProjectJson() {};
	FDataProjectJson(TMap<FString, FPlot> plots_) {
		plots = plots_;
	};
};

USTRUCT()
struct FContentMeshModel {
	GENERATED_BODY()

	UPROPERTY()
	TArray<float> size;
	UPROPERTY()
	FString type;
	UPROPERTY()
	FString path;
	UPROPERTY()
	FString materialPath;

	FContentMeshModel() {};
	FContentMeshModel(TArray<float> size_, FString type_, FString path_,
		FString materialPath_ = "") {
		size = size_;
		type = type_;
		path = path_;
		materialPath = materialPath_;
	};
};

USTRUCT()
struct FDataContentMesh {
	GENERATED_BODY()

	UPROPERTY()
	TMap<FString, FContentMeshModel> models;

	FDataContentMesh() {};
	FDataContentMesh(TMap<FString, FContentMeshModel> models_) {
		models = models_;
	};
};
