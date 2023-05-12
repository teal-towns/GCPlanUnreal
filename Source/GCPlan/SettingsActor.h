// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SettingsActor.generated.h"

UCLASS()
class GCPLAN_API ASettingsActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASettingsActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	bool Inited = false;

	void Init();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// UPROPERTY(EditAnywhere)
	// FString UrlWebsocket = "wss://plan.greencity.earth/ws";

	UPROPERTY(EditAnywhere)
	FString LoginEmail = "";

	UPROPERTY(EditAnywhere)
	FString LoginPassword = "";

	UPROPERTY(EditAnywhere)
	FString ProjectUName = "";
};
