#include "SocketActor.h"
#include "WebSocketsModule.h"
#include "Serialization/JsonSerializer.h"
#include "JsonObjectConverter.h"
#include "GCPlanGameInstance.h"
#include "Containers/UnrealString.h"
#include <string>
// #include "utf8.h"

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
	UGCPlanGameInstance* GameInstance = Cast<UGCPlanGameInstance>(GetGameInstance());
	if (GameInstance) {
		Inited = true;
		InitSocket();
		GameInstance->InitActor("socket");
	}
}

void ASocketActor::InitSocket() {
	if (!FModuleManager::Get().IsModuleLoaded("WebSockets")) {
        FModuleManager::Get().LoadModule("WebSockets");
    }
    FString Url = "wss://plan.greencity.earth/ws";
	// FString Url = "ws://localhost:8087/ws";
    WebSocket = FWebSocketsModule::Get().CreateWebSocket(Url);

    WebSocket->OnConnected().AddLambda([]() {
		UE_LOG(LogTemp, Display, TEXT("Websocket connected"));
        // GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, "Websocket connected");
    });
    WebSocket->OnConnectionError().AddLambda([](const FString& Error) {
		UE_LOG(LogTemp, Error, TEXT("Websocket error %s"), *Error);
        // GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "Websocket error " + Error);
    });
    WebSocket->OnClosed().AddLambda([](int32 StatusCode, const FString& Reason, bool bWasClean) {
		UE_LOG(LogTemp, Warning, TEXT("Websocket closed %s"), *Reason);
        // GEngine->AddOnScreenDebugMessage(-1, 15.0f, bWasClean ? FColor::Green : FColor::Red, "Websocket closed " + Reason);
    });
    WebSocket->OnMessage().AddLambda([this](const FString& MessageString) {
		UE_LOG(LogTemp, Display, TEXT("Websocket OnMessage %s"), *MessageString);
        // GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Cyan, "Websocket OnMessage" + MessageString);
		this->HandleMessage(MessageString);
    });
    WebSocket->OnRawMessage().AddLambda([this](const void* Data, SIZE_T Size, SIZE_T BytesRemaining) {
		// const FString* MessageString = Cast<FString>(Data);
		// FString MessageString = BytesToString((uint8*)Data, Size);
		// FUTF8ToTCHAR ToTCharConverter((uint8*)Data);
		// FString MessageString(ToTCharConverter.Get());

		// std::string line = (uint8*)Data;
		// char line[] = (uint8*)Data;
		uint8* line = (uint8*)Data;
		// std::string::iterator end_it = utf8::find_invalid(line.begin(), line.end());
		// UE_LOG(LogTemp, Display, TEXT("end_it %s end %s"), Size, BytesRemaining, *StartString, *EndString);
		// string utf8line = string(line.begin(), end_it);
		// FString MessageString = UTF8_TO_TCHAR(utf8line);

		// UE_LOG(LogTemp, Display, TEXT("raw data %d %d %d %d %s"), line[0], line[1], line[(Size - 1)], line[(Size - 2)], line);
		// Have to manually convert character by character, otherwise get invalid characters at the end..
		FString ValidString = "";
		for (int ii = 0; ii < Size; ii++) {
			if (line[ii] >= 0 && line[ii] < 128) {
				ValidString += char(line[ii]);
			}
		}

		// char DataChars[] = {};
		// for (int ii = 0; ii < Size; ii++) {
		// 	if (int(Data[ii]) > 0 && int(Data[ii]) < 128) {
		// 		DataChars += Data[ii];
		// 	}
		// }
		// FString ValidString = UTF8_TO_TCHAR(DataChars);

		// FString MessageString = UTF8_TO_TCHAR((uint8*)Data);
		// FString StartString = MessageString.Mid(0, 30);
		// FString EndString = MessageString.Mid(MessageString.Len() - 50, 50);
		// UE_LOG(LogTemp, Display, TEXT("OnRawMessage Size %d BytesRemaining %d |%s| |%s|"), Size, BytesRemaining, *StartString, *EndString);

		// FString ValidString = "";
		// for (int ii = 0; ii < MessageString.Len(); ii++) {
		// 	if (int(MessageString[ii]) >= 0 && int(MessageString[ii]) < 128) {
		// 		ValidString += MessageString[ii];
		// 	}
		// }
		// FString StartStringValid = ValidString.Mid(0, 30);
		// FString EndStringValid = ValidString.Mid(ValidString.Len() - 50, 50);
		// UE_LOG(LogTemp, Display, TEXT("|%s| |%s|"), *StartStringValid, *EndStringValid);

		// If over frame size, need to join the streamed in data together first.
		// TODO - this assumes messages are all from the same source.. What if another message comes in between frames??
		if (BytesRemaining > 0) {
			RawMessageJoiner += ValidString;
		} else {
			if (RawMessageJoiner.Len() > 0) {
				ValidString = RawMessageJoiner + ValidString;
			}
			// FString MessageString = UTF8_TO_TCHAR((uint8*)Data);
			UE_LOG(LogTemp, Display, TEXT("Websocket OnRawMessage %d %s"), ValidString.Len(), *ValidString);
			// GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Cyan, "Websocket OnRawMessage");
			// GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Cyan, *MessageString);
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

void ASocketActor::Close() {
	if (WebSocket->IsConnected()) {
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
	// FPlayerA Player;
	// Player.Name = "Frank";
	// Player.Level = 3;
	// Player.Friends.Add("Jeff");
	// Player.PropertyToIgnore = "Gotta Go Fast";

	// FString JSONPayload;
	// FJsonObjectConverter::UStructToJsonObjectString(Player, JSONPayload, 0, 0);

	FString JsonString;
	// TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
	// for (auto& Elem : Data) {
	// 	JsonObject->SetStringField(Elem.Key, *Elem.Value);
	// }
	// if (!FJsonSerializer::Serialize(JsonObject.ToSharedRef(), TJsonWriterFactory<>::Create(&JsonString))) {
	// SocketData SendObj = SocketData(Route, Data, GetAuth());
	// struct {
	// 	FString Route;
	// 	TMap<FString, FString> Data;
	// 	TMap<FString, FString> Auth;
	// } SendObj = { Route, Data, GetAuth() };
	// FSocketData* SendObj = new FSocketData(Route, Data, GetAuth());
	FSocketData SendObj;
	SendObj.Route = Route;
	SendObj.Data = Data;
	SendObj.Auth = GetAuth();
	if (!FJsonObjectConverter::UStructToJsonObjectString(SendObj, JsonString, 0, 0)) {
		UE_LOG(LogTemp, Error, TEXT("SocketActor.Emit JsonString error %s"), *Route);
		// GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "SocketActor.Emit JsonString error " + Route);
	} else {
		// TODO - ensure socket is open
		// GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, "SocketActor.Emit " + JsonString);
		UE_LOG(LogTemp, Display, TEXT("SocketActor.Emit Route %s"), *Route);
		WebSocket->Send(JsonString);
	}
}

// void ASocketActor::On(FString Route, [](FString) Callback) {
// void ASocketActor::On(FString Route) {
void ASocketActor::On(FString Route, std::function<void(FString)> Callback) {
	if (!ListenersOn.Contains(Route)) {
		ListenersOn.Add(Route, Callback);
	} else {
		UE_LOG(LogTemp, Error, TEXT("SocketActor.On route already exists - need to update to TArray to support multiple listeners per route!"));
		// GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "SocketActor.On route already exists - need to update to TArray so can have more than one listener! " + Route);
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
	// FString DataString1 = Message.LeftChop((Message.Len() - IndexEnd));
	// UE_LOG(LogTemp, Display, TEXT("after LeftChop %d %s"), DataString1.Len(), *DataString1);
	// DataString1 = DataString1.RightChop(Start);
	// UE_LOG(LogTemp, Display, TEXT("after RightChop %d %s"), DataString1.Len(), *DataString1);
	// UE_LOG(LogTemp, Warning, TEXT("Len %d Start %d IndexEnd %d SliceLen %d %d %d"), Message.Len(), Start, IndexEnd, SliceLen, (Message.Len() - IndexEnd), (Start + SliceLen));
	// UE_LOG(LogTemp, Display, TEXT("Route %s Data %d %s"), *Route, DataString.Len(), *DataString);
	// TESTING
	// DataString = "{\"valid\": 1, \"msg\": \"\"}";
	// FString EndString = Message.Mid(DataString.Len() - 100, 100);
	// UE_LOG(LogTemp, Display, TEXT("EndString %s"), *EndString);
	// FString EndStringFull = Message.Mid(Message.Len() - 200, 200);
	// UE_LOG(LogTemp, Display, TEXT("EndStringFull %s"), *EndStringFull);

	// FString EndString1 = Message.Mid(DataString1.Len() - 100, 100);
	// UE_LOG(LogTemp, Display, TEXT("EndString1 %s"), *EndString1);
	// // GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, "Route " + Route + " DataString " + DataString);
	if (ListenersOn.Contains(Route)) {
		ListenersOn[Route](DataString);
	}
}
