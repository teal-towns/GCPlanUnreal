#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "IWebSocket.h"
#include "SocketActor.generated.h"

UCLASS()
class GCPLAN_API ASocketActor : public AActor
{
	GENERATED_BODY()

public:	
	ASocketActor();
	virtual void Tick(float DeltaTime) override;

	TSharedPtr<IWebSocket> WebSocket;

	void Close();
	bool IsConnected();
	void Emit(FString, TMap<FString, FString>);
	FString On(FString, FString, std::function<void(FString)>);
	void Off(FString);
	void InitSocket(bool closeSocket = false);
	void Destroy();

protected:
	bool Inited = false;
	// TMap<FString, [](FString)> ListenersOn;
	TMap<FString, std::function<void(FString)>> ListenersOn;
	// template <typename T>
	// TMap<FString, T> ListenersOn;
	FString RawMessageJoiner = "";

	void Init();
	void HandleMessage(FString);
	TMap<FString, FString> GetAuth();

	// struct SocketData {
	// 	FString Route;
	// 	TMap<FString, FString> Data;
	// 	TMap<FString, FString> Auth;
	// };
};

USTRUCT()
struct FSocketData {
	GENERATED_BODY()

	UPROPERTY()
	FString Route;
	UPROPERTY()
	TMap<FString, FString> Data;
	UPROPERTY()
	TMap<FString, FString> Auth;

	// FSocketData(FString Route = "", TMap<FString, FString> Data = {}, TMap<FString, FString> Auth = {}) {}
};
