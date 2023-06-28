#pragma once

#include "../ModelBase.h"

class ModelTile {
public:
	ModelTile();
	~ModelTile();

	static void Left(AStaticMeshActor* actor, FString name, FVector location, FVector rotation,
					FVector scale, FActorSpawnParameters spawnParams, FModelParams modelParams);
	static void Right(AStaticMeshActor* actor, FString name, FVector location, FVector rotation,
					FVector scale, FActorSpawnParameters spawnParams, FModelParams modelParams);
	static void Back(AStaticMeshActor* actor, FString name, FVector location, FVector rotation,
					FVector scale, FActorSpawnParameters spawnParams, FModelParams modelParams);
	static void Front(AStaticMeshActor* actor, FString name, FVector location, FVector rotation,
					FVector scale, FActorSpawnParameters spawnParams, FModelParams modelParams);
	static void Bottom(AStaticMeshActor* actor, FString name, FVector location, FVector rotation,
					FVector scale, FActorSpawnParameters spawnParams, FModelParams modelParams);
	static void Top(AStaticMeshActor* actor, FString name, FVector location, FVector rotation,
					FVector scale, FActorSpawnParameters spawnParams, FModelParams modelParams);
	static void doTile(AStaticMeshActor* actor, FString name, FVector location, FVector rotation,
		FVector size, FActorSpawnParameters spawnParams, FModelParams modelParams, FVector tile);
};
