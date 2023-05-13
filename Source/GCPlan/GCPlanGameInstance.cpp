#include "GCPlanGameInstance.h"
#include "SettingsActor.h"
#include "SocketActor.h"
#include "Kismet/GameplayStatics.h"

void UGCPlanGameInstance::Init() {
    Super::Init();
}

void UGCPlanGameInstance::Shutdown() {
    SocketActor->Close();
    Super::Shutdown();
}

void UGCPlanGameInstance::InitActor(FString name) {
    TArray<AActor*> OutActors;
    if (name == "settings") {
        TArray<ASettingsActor*> MyActors;
        UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASettingsActor::StaticClass(), OutActors);
        for (AActor* a : OutActors) {
            SettingsActor = Cast<ASettingsActor>(a);
            break;
        }
        Initeds.Add("settings", true);
    } else if (name == "socket") {
        TArray<ASocketActor*> MyActors;
        UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASocketActor::StaticClass(), OutActors);
        for (AActor* a : OutActors) {
            SocketActor = Cast<ASocketActor>(a);
            break;
        }
        Initeds.Add("socket", true);
    }
}

bool UGCPlanGameInstance::IsIniteds(TArray<FString> Keys) {
    for (int ii = 0; ii < Keys.Num(); ii++) {
        if (!Initeds.Contains(Keys[ii]) || !Initeds[Keys[ii]]) {
            return false;
        }
    }
    return true;
}
