#include "LandProjectActor.h"
#include "JsonObjectConverter.h"
#include "InstancedStaticMeshActor.h"
#include "Engine/World.h"
#include "GCPlanGameInstance.h"

#include "Kismet/GameplayStatics.h"
#include "InstancedStaticMeshActor.h"
#include "SettingsActor.h"
#include "SocketActor.h"

#include "GlobalClass.h"

ALandProjectActor::ALandProjectActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

void ALandProjectActor::BeginPlay()
{
	Super::BeginPlay();
}

void ALandProjectActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!Inited) {
		Init();
	}
}

void ALandProjectActor::InitSocketOn() {
	SocketActor->On("login", [this](FString DataString) {
		FDataLogin* Data = new FDataLogin();
		if (!FJsonObjectConverter::JsonObjectStringToUStruct(DataString, Data, 0, 0)) {
			UE_LOG(LogTemp, Error, TEXT("LandProjectActor.On login json parse error"));
			// GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "LandProjectActor.On login json parse error");
		} else {
			if (Data->valid > 0) {
				FString ProjectUName = SettingsActor->ProjectUName;
				if (ProjectUName.Len() > 0) {
					this->GetProject(ProjectUName);
				}
			}
		}
	});

	SocketActor->On("projectGetByUName", [this](FString DataString) {
		FDataProject* Data = new FDataProject();
		if (!FJsonObjectConverter::JsonObjectStringToUStruct(DataString, Data, 0, 0)) {
			UE_LOG(LogTemp, Error, TEXT("LandProjectActor.On projectGetByUName json parse error"));
			// GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "LandProjectActor.On projectGetByUName json parse error");
		} else {
			if (Data->valid > 0) {
				this->GenerateLands(Data->lands);
			}
		}
	});
}

void ALandProjectActor::Login() {
	FString LoginEmail = SettingsActor->LoginEmail;
	FString LoginPassword = SettingsActor->LoginPassword;
	FString ProjectUName = SettingsActor->ProjectUName;
	if (LoginEmail.Len() > 0 && LoginPassword.Len() > 0 && ProjectUName.Len() > 0) {
		TMap<FString, FString> Data = {
			{ "email", LoginEmail },
			{ "password", LoginPassword }
		};
		SocketActor->Emit("login", Data);
	}
}

void ALandProjectActor::Init() {
	UGCPlanGameInstance* GameInstance = Cast<UGCPlanGameInstance>(GetGameInstance());
	TArray<FString> Keys = {"settings", "socket"};
	if (GameInstance && GameInstance->IsIniteds(Keys) && GameInstance->SocketActor->IsConnected()) {
		Inited = true;
		// ASettingsActor* SettingsActor = GameInstance->SettingsActor;
		SettingsActor = GameInstance->SettingsActor;

		SocketActor = GameInstance->SocketActor;
		HexActor = GameInstance->GetInstancedStaticMeshActor("HexModule");

		this->InitSocketOn();
		this->Login();
	}
}

void ALandProjectActor::GetProject(FString UName) {
	TMap<FString, FString> Data = {
		{ "uName", UName },
		{ "getPlotsSupercells", "1" }
	};
	SocketActor->Emit("projectGetByUName", Data);
}

void ALandProjectActor::GenerateLands(TArray<FLand> lands) {
	HexActor->ClearInstances();
	// UGCPlanGameInstance* GameInstance = Cast<UGCPlanGameInstance>(GetGameInstance());
	// AInstancedStaticMeshActor* HexActor = GameInstance->GetInstancedStaticMeshActor("HexModule");
	// TODO - Unity hex prefab was rotated; reset to 0 rotation as default then remove this.
	float rotationYOffset = 30.0;
	// Move up to be above land.
	float zOffset = 25000.0;
	for (int ii = 0; ii < lands.Num(); ii++) {
		UE_LOG(LogTemp, Display, TEXT("land_id %s"), *lands[ii].land_id);
		for (auto& Elem : lands[ii].game_objects) {
			FLandGameObject GO = Elem.Value;
			// Stored in Unity style so swap z and y and convert from meters to centimeters.
			FVector Translation = FVector(GO.position["x"] * 100.0, GO.position["z"] * 100.0, GO.position["y"] * 100.0 + zOffset);
			// FVector Translation = FVector(GO.position["x"] * 10.0, GO.position["z"] * 10.0, GO.position["y"] * 10.0);
			// FVector Translation = FVector(GO.position["x"], GO.position["z"], GO.position["y"]);
			FRotator Rotation = FRotator(GO.rotation["x"], GO.rotation["y"] + rotationYOffset, GO.rotation["z"]);
			FVector Scale = FVector(GO.scale["x"], GO.scale["y"], GO.scale["z"]);
			HexActor->CreateInstance(Translation, Rotation, Scale);
		}
	}
}

// TODO - abstract this to GlobalClass or somewhere else - need to find an Unreal singleton in editor mode (in place of GameInstance).
void ALandProjectActor::EditorGenerate() {
	UObject* world = GetWorld();

	TArray<AActor*> OutActors;
    UGameplayStatics::GetAllActorsOfClass(world, ASettingsActor::StaticClass(), OutActors);
    for (AActor* a : OutActors) {
        SettingsActor = Cast<ASettingsActor>(a);
        break;
    }
    UGameplayStatics::GetAllActorsOfClass(world, ASocketActor::StaticClass(), OutActors);
    for (AActor* a : OutActors) {
        SocketActor = Cast<ASocketActor>(a);
        break;
    }
    UGameplayStatics::GetAllActorsOfClass(world, AInstancedStaticMeshActor::StaticClass(), OutActors);
    for (AActor* a : OutActors) {
        AInstancedStaticMeshActor* Actor = Cast<AInstancedStaticMeshActor>(a);
        FString Name = Actor->GetName();
        InstancedStaticMeshActors.Add(Name, Actor);
    }

    SocketActor->InitSocket();

    HexActor = InstancedStaticMeshActors["HexModule"];

    this->InitSocketOn();
	this->Login();
}
