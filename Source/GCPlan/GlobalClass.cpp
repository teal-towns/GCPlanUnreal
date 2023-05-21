// NOT CURRENTLY USED

#include "GlobalClass.h"
#include "Kismet/GameplayStatics.h"
#include "InstancedStaticMeshActor.h"
#include "SettingsActor.h"
#include "SocketActor.h"

GlobalClass::GlobalClass()
{
}

GlobalClass::~GlobalClass()
{
}

// AInstancedStaticMeshActor* Actor GetInstancedStaticMeshActor(FString name) {
// 	UGCPlanGameInstance* GameInstance = Cast<UGCPlanGameInstance>(GetGameInstance());
// 	if (GameInstance) {
// 		return GameInstance->GetInstancedStaticMeshActor(name);
// 	}
// 	return TODO
// }

void GlobalClass::InitActor(FString name, UObject* world) {
    TArray<AActor*> OutActors;
    if (name == "settings") {
        TArray<ASettingsActor*> MyActors;
        UGameplayStatics::GetAllActorsOfClass(world, ASettingsActor::StaticClass(), OutActors);
        for (AActor* a : OutActors) {
            SettingsActor = Cast<ASettingsActor>(a);
            break;
        }
        Initeds.Add("settings", true);
    } else if (name == "socket") {
        TArray<ASocketActor*> MyActors;
        UGameplayStatics::GetAllActorsOfClass(world, ASocketActor::StaticClass(), OutActors);
        for (AActor* a : OutActors) {
            SocketActor = Cast<ASocketActor>(a);
            break;
        }
        Initeds.Add("socket", true);
    }
    // Init others too that should not need specific async timing.
    if (!Initeds.Contains("other")) {
        UGameplayStatics::GetAllActorsOfClass(world, AInstancedStaticMeshActor::StaticClass(), OutActors);
        for (AActor* a : OutActors) {
            AInstancedStaticMeshActor* Actor = Cast<AInstancedStaticMeshActor>(a);
            FString Name = Actor->GetName();
            InstancedStaticMeshActors.Add(Name, Actor);
        }
        Initeds.Add("other", true);
    }
}

bool GlobalClass::IsIniteds(TArray<FString> Keys) {
    for (int ii = 0; ii < Keys.Num(); ii++) {
        if (!Initeds.Contains(Keys[ii]) || !Initeds[Keys[ii]]) {
            return false;
        }
    }
    return true;
}

AInstancedStaticMeshActor* GlobalClass::GetInstancedStaticMeshActor(FString Name) {
    if (InstancedStaticMeshActors.Contains(Name)) {
        return InstancedStaticMeshActors[Name];
    }
    return nullptr;
}
