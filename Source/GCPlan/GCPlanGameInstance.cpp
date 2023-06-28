#include "GCPlanGameInstance.h"
#include "Kismet/GameplayStatics.h"

// #include "SocketActor.h"
#include "Common/UnrealGlobal.h"

void UGCPlanGameInstance::Init() {
	Super::Init();
	UnrealGlobal* unrealGlobal = UnrealGlobal::GetInstance();
	TArray<FString> skipKeys = { "web" };
	// TArray<FString> skipKeys = {};
	unrealGlobal->InitAll(GetWorld(), skipKeys);
}

void UGCPlanGameInstance::Shutdown() {
	// UnrealGlobal* unrealGlobal = UnrealGlobal::GetInstance();
	// unrealGlobal->CleanUp();
	Super::Shutdown();
}

// void UGCPlanGameInstance::InitActor(FString name) {
// 	TArray<AActor*> OutActors;
// 	if (name == "socket") {
// 		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASocketActor::StaticClass(), OutActors);
// 		for (AActor* a : OutActors) {
// 			SocketActor = Cast<ASocketActor>(a);
// 			break;
// 		}
// 		Initeds.Add("socket", true);
// 	}
// }

// bool UGCPlanGameInstance::IsIniteds(TArray<FString> Keys) {
// 	for (int ii = 0; ii < Keys.Num(); ii++) {
// 		if (!Initeds.Contains(Keys[ii]) || !Initeds[Keys[ii]]) {
// 			return false;
// 		}
// 	}
// 	return true;
// }
