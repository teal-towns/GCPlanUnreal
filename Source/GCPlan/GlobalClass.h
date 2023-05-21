// NOT CURRENTLY USED

#pragma once

#include "CoreMinimal.h"

#include "InstancedStaticMeshActor.h"
#include "SettingsActor.h"
#include "SocketActor.h"

class GCPLAN_API GlobalClass
{
public:
	GlobalClass();
	~GlobalClass();

	TMap<FString, AInstancedStaticMeshActor*> InstancedStaticMeshActors;
	ASettingsActor* SettingsActor;
	ASocketActor* SocketActor;
	TMap<FString, bool> Initeds;

	void InitActor(FString, UObject*);
	bool IsIniteds(TArray<FString>);
	AInstancedStaticMeshActor* GetInstancedStaticMeshActor(FString);
};
