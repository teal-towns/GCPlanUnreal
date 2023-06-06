#include "GCPlanGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "SettingsActor.h"
#include "SocketActor.h"

void UGCPlanGameInstance::Init() {
    Super::Init();
}

void UGCPlanGameInstance::Shutdown() {
    SocketActor->Destroy();
    Super::Shutdown();
}

void UGCPlanGameInstance::InitActor(FString name) {
    TArray<AActor*> OutActors;
    if (name == "settings") {
        UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASettingsActor::StaticClass(), OutActors);
        for (AActor* a : OutActors) {
            SettingsActor = Cast<ASettingsActor>(a);
            break;
        }
        Initeds.Add("settings", true);
    } else if (name == "socket") {
        UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASocketActor::StaticClass(), OutActors);
        for (AActor* a : OutActors) {
            SocketActor = Cast<ASocketActor>(a);
            break;
        }
        Initeds.Add("socket", true);
    }
    // Init others too that should not need specific async timing.
    if (!Initeds.Contains("other")) {
        Initeds.Add("other", true);
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
