#include "SocketActor.h"
#include "WebSocketsModule.h"
#include "Serialization/JsonSerializer.h"
#include "JsonObjectConverter.h"
#include "GCPlanGameInstance.h"
#include "Containers/UnrealString.h"
#include <string>
// #include "utf8.h"

#include "Common/UnrealGlobal.h"

ASocketActor::ASocketActor()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ASocketActor::BeginPlay()
{
	Super::BeginPlay();
}

void ASocketActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!Inited) {
		Init();
	}
}

void ASocketActor::Init() {
	UnrealGlobal* unrealGlobal = UnrealGlobal::GetInstance();
	UGCPlanGameInstance* GameInstance = Cast<UGCPlanGameInstance>(GetGameInstance());
	if (GameInstance) {
		Inited = true;
		InitSocket();
		unrealGlobal->GetSocket(GetWorld());
		unrealGlobal->SetInited("socket");
		// GameInstance->InitActor("socket");
	}
}

void ASocketActor::InitSocket() {
	if (!FModuleManager::Get().IsModuleLoaded("WebSockets")) {
		FModuleManager::Get().LoadModule("WebSockets");
	}
	this->Destroy();

	UnrealGlobal* unrealGlobal = UnrealGlobal::GetInstance();
	FString Url = unrealGlobal->_settings->urlWebsocket;
	WebSocket = FWebSocketsModule::Get().CreateWebSocket(Url);

	WebSocket->OnConnected().AddLambda([]() {
		UE_LOG(LogTemp, Display, TEXT("Websocket connected"));
	});
	WebSocket->OnConnectionError().AddLambda([](const FString& Error) {
		UE_LOG(LogTemp, Error, TEXT("Websocket error %s"), *Error);
	});
	WebSocket->OnClosed().AddLambda([](int32 StatusCode, const FString& Reason, bool bWasClean) {
		UE_LOG(LogTemp, Warning, TEXT("Websocket closed %s"), *Reason);
	});
	WebSocket->OnMessage().AddLambda([this](const FString& MessageString) {
		// UE_LOG(LogTemp, Display, TEXT("Websocket OnMessage %s"), *MessageString);
		this->HandleMessage(MessageString);
	});
	WebSocket->OnRawMessage().AddLambda([this](const void* Data, SIZE_T Size, SIZE_T BytesRemaining) {
		uint8* line = (uint8*)Data;
		FString ValidString = "";
		for (int ii = 0; ii < Size; ii++) {
			if (line[ii] >= 0 && line[ii] < 128) {
				ValidString += char(line[ii]);
			}
		}
		// TODO - this assumes messages are all from the same source.. What if another message comes in between frames??
		if (BytesRemaining > 0) {
			RawMessageJoiner += ValidString;
		} else {
			if (RawMessageJoiner.Len() > 0) {
				ValidString = RawMessageJoiner + ValidString;
			}
			// UE_LOG(LogTemp, Display, TEXT("Websocket OnRawMessage %d %s"), ValidString.Len(), *ValidString);
			this->HandleMessage(ValidString);
			// Reset for next time.
			RawMessageJoiner = "";
		}
	});
	WebSocket->OnMessageSent().AddLambda([](const FString& MessageString) {
		// GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, "Websocket sent message" + MessageString);
	});
	WebSocket->Connect();
}

void ASocketActor::Destroy() {
	ListenersOn.Empty();
	this->Close();
}

void ASocketActor::Close() {
	if (WebSocket && WebSocket->IsConnected()) {
		WebSocket->Close();
	}
}

bool ASocketActor::IsConnected() {
	return WebSocket->IsConnected();
}

TMap<FString, FString> ASocketActor::GetAuth() {
	TMap<FString, FString> Auth = {
		{ "user_id", "" },
		{ "session_id", "" }
	};
	return Auth;
}

void ASocketActor::Emit(FString Route, TMap<FString, FString> Data) {
	FString JsonString;
	FSocketData SendObj;
	SendObj.Route = Route;
	SendObj.Data = Data;
	SendObj.Auth = GetAuth();
	if (!FJsonObjectConverter::UStructToJsonObjectString(SendObj, JsonString, 0, 0)) {
		UE_LOG(LogTemp, Error, TEXT("SocketActor.Emit JsonString error %s"), *Route);
	} else {
		// TODO - ensure socket is open
		UE_LOG(LogTemp, Display, TEXT("SocketActor.Emit Route %s"), *Route);
		WebSocket->Send(JsonString);
	}
}

void ASocketActor::On(FString Route, std::function<void(FString)> Callback) {
	if (!ListenersOn.Contains(Route)) {
		ListenersOn.Add(Route, Callback);
	} else {
		UE_LOG(LogTemp, Error, TEXT("SocketActor.On route already exists - need to update to TArray to support multiple listeners per route!"));
	}
}

void ASocketActor::Off(FString Route) {
	if (ListenersOn.Contains(Route)) {
		ListenersOn.Remove(Route);
	}
}

void ASocketActor::HandleMessage(FString Message) {
	// Find route.
	FString Word = "\"route\": \"";
	int IndexWord = Message.Find(Word);
	int Start = (IndexWord + Word.Len());
	// Could be long messages (4 million+ len, so need 32 to 64 bit; 16bit only goes to 65k)
	uint64 IndexEnd = Message.Find("\",", ESearchCase::CaseSensitive, ESearchDir::FromStart, Start);
	uint64 SliceLen = IndexEnd - Start;
	FString Route = Message.Mid(Start, SliceLen);

	// Get data
	Word = "\"data\": ";
	IndexWord = Message.Find(Word);
	Start = (IndexWord + Word.Len());
	IndexEnd = Message.Find(", \"auth\":", ESearchCase::CaseSensitive, ESearchDir::FromStart, Start);
	SliceLen = IndexEnd - Start;
	FString DataString = Message.Mid(Start, SliceLen);
	if (ListenersOn.Contains(Route)) {
		ListenersOn[Route](DataString);
	}
}
