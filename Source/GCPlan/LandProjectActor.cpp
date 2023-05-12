#include "LandProjectActor.h"
// #include "SettingsActor.h"
#include "GCPlanGameInstance.h"

// Sets default values
ALandProjectActor::ALandProjectActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ALandProjectActor::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ALandProjectActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!Inited) {
		Init();
	}
}

void ALandProjectActor::Init() {
	UGCPlanGameInstance* GameInstance = Cast<UGCPlanGameInstance>(GetGameInstance());
	TArray<FString> Keys;
	// Keys.Init("settings");
	Keys.Emplace("settings");
	if (GameInstance && GameInstance->WebSocket->IsConnected() && GameInstance->IsIniteds(Keys)) {
		Inited = true;
		ASettingsActor* SettingsActor = GameInstance->SettingsActor;
		FString LoginEmail = SettingsActor->LoginEmail;
		FString LoginPassword = SettingsActor->LoginPassword;
		FString ProjectUName = SettingsActor->ProjectUName;
		if (LoginEmail.Len() > 0 && LoginPassword.Len() > 0 && ProjectUName.Len() > 0) {
			// GameInstance->WebSocket->Send();
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, "Logging in and getting project " +
				LoginEmail + " " + ProjectUName);
		}
	}
}

