// NOT CURRENTLY USED

#include "GCPlanEngine.h"
// #include "Kismet/GameplayStatics.h"
// #include "InstancedStaticMeshActor.h"
// #include "SettingsActor.h"
// #include "SocketActor.h"

// void UGCPlanEngine::InitActor(FString name) {
//     TArray<AActor*> OutActors;
//     if (name == "settings") {
//         TArray<ASettingsActor*> MyActors;
//         UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASettingsActor::StaticClass(), OutActors);
//         for (AActor* a : OutActors) {
//             SettingsActor = Cast<ASettingsActor>(a);
//             break;
//         }
//         Initeds.Add("settings", true);
//     } else if (name == "socket") {
//         TArray<ASocketActor*> MyActors;
//         UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASocketActor::StaticClass(), OutActors);
//         for (AActor* a : OutActors) {
//             SocketActor = Cast<ASocketActor>(a);
//             break;
//         }
//         Initeds.Add("socket", true);
//     }
//     // Init others too that should not need specific async timing.
//     if (!Initeds.Contains("other")) {
//         UGameplayStatics::GetAllActorsOfClass(GetWorld(), AInstancedStaticMeshActor::StaticClass(), OutActors);
//         for (AActor* a : OutActors) {
//             AInstancedStaticMeshActor* Actor = Cast<AInstancedStaticMeshActor>(a);
//             FString Name = Actor->GetName();
//             InstancedStaticMeshActors.Add(Name, Actor);
//         }
//         Initeds.Add("other", true);
//     }
// }

// bool UGCPlanEngine::IsIniteds(TArray<FString> Keys) {
//     for (int ii = 0; ii < Keys.Num(); ii++) {
//         if (!Initeds.Contains(Keys[ii]) || !Initeds[Keys[ii]]) {
//             return false;
//         }
//     }
//     return true;
// }

// AInstancedStaticMeshActor* UGCPlanEngine::GetInstancedStaticMeshActor(FString Name) {
//     if (InstancedStaticMeshActors.Contains(Name)) {
//         return InstancedStaticMeshActors[Name];
//     }
//     return nullptr;
// }
