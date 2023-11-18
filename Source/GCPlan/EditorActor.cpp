#include "EditorActor.h"

#include "Common/UnrealGlobal.h"
#include "Landscape/VectorTiles.h"

AEditorActor::AEditorActor()
{
}

void AEditorActor::BeginPlay()
{
	Super::BeginPlay();
	// GetTiles();
}

void AEditorActor::GetTiles() {
	UE_LOG(LogTemp, Display, TEXT("AEditorActor::GetTiles"));
	UnrealGlobal* unrealGlobal = UnrealGlobal::GetInstance();
	unrealGlobal->InitAll(GetWorld());
	if (unrealGlobal->SocketActor != nullptr && unrealGlobal->SocketActor->IsConnected()) {
		// this->Login();
		VectorTiles* vectorTiles = VectorTiles::GetInstance();
		float lng = -122.033802;
		float lat = 37.977362;
		vectorTiles->GetTiles(lng, lat, 125, 125);
	} else {
		UE_LOG(LogTemp, Warning, TEXT("EditorActor.GetTiles socket not connected"));
	}
}
