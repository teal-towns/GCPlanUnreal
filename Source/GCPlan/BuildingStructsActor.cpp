// Fill out your copyright notice in the Description page of Project Settings.


#include "BuildingStructsActor.h"

// Sets default values
ABuildingStructsActor::ABuildingStructsActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ABuildingStructsActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABuildingStructsActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

