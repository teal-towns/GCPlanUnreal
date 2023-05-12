// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "IWebSocket.h"
#include "GCPlanGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class GCPLAN_API UGCPlanGameInstance : public UGameInstance
{
	GENERATED_BODY()

	public:
		virtual void Init() override;
		virtual void Shutdown() override;

		TSharedPtr<IWebSocket> WebSocket;
};
