#include "GCPlanGameInstance.h"
#include "WebSocketsModule.h"

void UGCPlanGameInstance::Init() {
    Super::Init();
    if (!FModuleManager::Get().IsModuleLoaded("WebSockets")) {
        FModuleManager::Get().LoadModule("WebSockets");
    }
    WebSocket = FWebSocketsModule::Get().CreateWebSocket("wss://plan.greencity.earth/ws");

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
