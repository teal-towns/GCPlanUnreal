#include "SettingsActor.h"
#include "GCPlanGameInstance.h"

// Sets default values
ASettingsActor::ASettingsActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ASettingsActor::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ASettingsActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!Inited) {
		Init();
	}
}

void ASettingsActor::Init() {
	UGCPlanGameInstance* GameInstance = Cast<UGCPlanGameInstance>(GetGameInstance());
	if (GameInstance && GameInstance->WebSocket->IsConnected()) {
		Inited = true;
		GameInstance->InitSettings();
	}
}

// FString& ASettingsActor::GetUrlWebsocket() {
// 	return UrlWebsocket;
// }
// FString& ASettingsActor::GetLoginEmail() {
// 	return LoginEmail;
// }
// FString& ASettingsActor::GetLoginPassword() {
// 	return LoginPassword;
// }
// FString& ASettingsActor::GetProjectUName() {
// 	return ProjectUName;
// }
