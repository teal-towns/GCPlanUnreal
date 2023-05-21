// NOT CURRENTLY USED

#pragma once

#include "CoreMinimal.h"
#include "Engine/Engine.h"
// #include "InstancedStaticMeshActor.h"
// #include "SettingsActor.h"
// #include "SocketActor.h"
#include "GCPlanEngine.generated.h"

UCLASS()
class GCPLAN_API UGCPlanEngine : public UEngine
{
	GENERATED_BODY()

	// public:
	// 	TMap<FString, AInstancedStaticMeshActor*> InstancedStaticMeshActors;
	// 	ASettingsActor* SettingsActor;
	// 	ASocketActor* SocketActor;
	// 	TMap<FString, bool> Initeds;

	// 	void InitActor(FString);
	// 	bool IsIniteds(TArray<FString>);
	// 	AInstancedStaticMeshActor* GetInstancedStaticMeshActor(FString);
};
