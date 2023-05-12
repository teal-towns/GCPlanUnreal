#include "GCPlanGameInstance.h"
#include "WebSocketsModule.h"
#include "SettingsActor.h"
#include "Kismet/GameplayStatics.h"

// TODO - move websocket code to separate file.

void UGCPlanGameInstance::Init() {
    Super::Init();
    if (!FModuleManager::Get().IsModuleLoaded("WebSockets")) {
        FModuleManager::Get().LoadModule("WebSockets");
    }
    FString Url = "wss://plan.greencity.earth/ws";
    WebSocket = FWebSocketsModule::Get().CreateWebSocket(Url);

    WebSocket->OnConnected().AddLambda([]() {
        GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, "Websocket connected");
    });
    WebSocket->OnConnectionError().AddLambda([](const FString& Error) {
        GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "Websocket error " + Error);
    });
    WebSocket->OnClosed().AddLambda([](int32 StatusCode, const FString& Reason, bool bWasClean) {
        GEngine->AddOnScreenDebugMessage(-1, 15.0f, bWasClean ? FColor::Green : FColor::Red, "Websocket closed " + Reason);
    });
    WebSocket->OnMessage().AddLambda([](const FString& MessageString) {
        GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Cyan, "Websocket OnMessage" + MessageString);
    });
    WebSocket->OnRawMessage().AddLambda([](const void* Data, SIZE_T Size, SIZE_T BytesRemaining) {
        GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Cyan, "Websocket OnRawMessage");
    });
    WebSocket->OnMessageSent().AddLambda([](const FString& MessageString) {
        GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, "Websocket sent message" + MessageString); 
    });
    WebSocket->Connect();
}

void UGCPlanGameInstance::Shutdown() {
    if (WebSocket->IsConnected()) {
        WebSocket->Close();
    }
    Super::Shutdown();
}

void UGCPlanGameInstance::InitSettings() {
    // SettingsActor = GetWorld()->SpawnActor<ASettingsActor>();
    // SettingsActor = Cast<ASettingsActor>(FindObject<AActor>(nullptr, TEXT("SettingsActor")));
    // Does not work? Always null..
    // SettingsActor = FindObject<ASettingsActor>(nullptr, TEXT("SettingsActor"));
    TArray<ASettingsActor*> MyActors;
    TArray<AActor*> OutActors;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASettingsActor::StaticClass(), OutActors);
    for (AActor* a : OutActors) {
        SettingsActor = Cast<ASettingsActor>(a);
        break;
    }
    Initeds.Add("settings", true);
}

bool UGCPlanGameInstance::IsIniteds(TArray<FString> Keys) {
    // bool AllInited = true;
    for (int ii = 0; ii < Keys.Num(); ii++) {
        if (!Initeds.Contains(Keys[ii]) || !Initeds[Keys[ii]]) {
            return false;
        }
    }
    return true;
}
