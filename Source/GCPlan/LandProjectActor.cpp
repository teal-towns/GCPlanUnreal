#include "LandProjectActor.h"
#include "JsonObjectConverter.h"
// #include "SettingsActor.h"
#include "GCPlanGameInstance.h"

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

void ALandProjectActor::Init() {
	UGCPlanGameInstance* GameInstance = Cast<UGCPlanGameInstance>(GetGameInstance());
	TArray<FString> Keys = {"settings", "socket"};
	if (GameInstance && GameInstance->IsIniteds(Keys) && GameInstance->SocketActor->IsConnected()) {
		Inited = true;

		ASettingsActor* SettingsActor = GameInstance->SettingsActor;

		GameInstance->SocketActor->On("login", [this, GameInstance, SettingsActor](FString DataString) {
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

		GameInstance->SocketActor->On("projectGetByUName", [this](FString DataString) {
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

		FString LoginEmail = SettingsActor->LoginEmail;
		FString LoginPassword = SettingsActor->LoginPassword;
		FString ProjectUName = SettingsActor->ProjectUName;
		if (LoginEmail.Len() > 0 && LoginPassword.Len() > 0 && ProjectUName.Len() > 0) {
			TMap<FString, FString> Data = {
				{ "email", LoginEmail },
				{ "password", LoginPassword }
			};
			GameInstance->SocketActor->Emit("login", Data);
		}
	}
}

void ALandProjectActor::GetProject(FString UName) {
	UGCPlanGameInstance* GameInstance = Cast<UGCPlanGameInstance>(GetGameInstance());
	TMap<FString, FString> Data = {
		{ "uName", UName },
		{ "getPlotsSupercells", "1" }
	};
	GameInstance->SocketActor->Emit("projectGetByUName", Data);
}

void ALandProjectActor::GenerateLands(TArray<FLand> lands) {
	for (int ii = 0; ii < lands.Num(); ii++) {
		UE_LOG(LogTemp, Display, TEXT("land_id %s"), *lands[ii].land_id);
		// GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, "land_id " + lands[ii].land_id);
		// TODO
	}
}
