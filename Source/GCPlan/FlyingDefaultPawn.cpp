#include "FlyingDefaultPawn.h"

#include "Blueprint/UserWidget.h"
#include "Engine.h"
#include "Input/Events.h"
#include "Kismet/KismetMathLibrary.h"
#include "Layout/Geometry.h"

#include "Common/Lodash.h"
#include "Common/UnrealGlobal.h"
#include "Landscape/HeightMap.h"
#include "Landscape/VerticesEdit.h"

void AFlyingDefaultPawn::BeginPlay() {
	Super::BeginPlay();
	// TESTING
	// CreateUI();
	// SetupMouse();
	// SetupRotation();
}

void AFlyingDefaultPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	SetMiniMapZoom();
}

void AFlyingDefaultPawn::CreateUI() {
	// Mini map
	// TODO - not working.
	// FString path = "/Script/UMGEditor.WidgetBlueprint'/Game/UI/MiniMapWidget_BP.MiniMapWidget_BP'";
	// UUserWidget* miniMap = Cast<UUserWidget>(StaticLoadObject(UUserWidget::StaticClass(), NULL,
	// 	*path));
	if (!MiniMap) {
		UE_LOG(LogTemp, Warning, TEXT("FlyingDefaultPawn.CreateUI missing MiniMap"));
	} else {
		MiniMap->AddToViewport(0);
		// FGeometry geometry;
		// FPointerEvent mouseEvent;
		// FEventReply reply = MiniMap->OnMouseButtonUp(geometry, mouseEvent);
	}

	if (!EditVerticesWidget) {
		UE_LOG(LogTemp, Warning, TEXT("FlyingDefaultPawn.CreateUI missing EditVerticesWidget"));
	} else {
		EditVerticesWidget->AddToViewport(1);
		EditVerticesWidget->Init();
	}
}

// https://forums.unrealengine.com/t/umg-onclick-event-for-non-button/485560/3
// void AFlyingDefaultPawn::OnMouseDownMiniMap(FGeometry geometry, const FPointerEvent& mouseEvent) {
// 	// FEventReply reply = Super::OnMouseButtonUp(geometry, mouseEvent);
// 	UE_LOG(LogTemp, Display, TEXT("mouse up %s"), *mouseEvent.GetScreenSpacePosition().ToString());
// 	// return reply;
// }
void AFlyingDefaultPawn::OnMouseDownMiniMap(const FGeometry& geometry, const FPointerEvent& mouseEvent) {
	auto [worldLocation, valid] = GetMouseLocation(geometry, mouseEvent);
	if (valid) {
		// UE_LOG(LogTemp, Display, TEXT("worldLocation %s"), *worldLocation.ToString());
		VerticesEdit* verticesEdit = VerticesEdit::GetInstance();
		verticesEdit->OnMouseDown(worldLocation);
	}
}

void AFlyingDefaultPawn::OnMouseUpMiniMap(const FGeometry& geometry, const FPointerEvent& mouseEvent) {
	auto [worldLocation, valid] = GetMouseLocation(geometry, mouseEvent);
	if (valid) {
		// UE_LOG(LogTemp, Display, TEXT("worldLocation %s"), *worldLocation.ToString());
		VerticesEdit* verticesEdit = VerticesEdit::GetInstance();
		verticesEdit->OnMouseUp(worldLocation);
	}
}

// void AFlyingDefaultPawn::OnDragStartMiniMap(const FGeometry& geometry, const FPointerEvent& mouseEvent) {
// 	UE_LOG(LogTemp, Display, TEXT("drag start"));
// 	VerticesEdit* verticesEdit = VerticesEdit::GetInstance();
// 	verticesEdit->OnDragStart();
// }

void AFlyingDefaultPawn::OnMouseMoveMiniMap(const FGeometry& geometry, const FPointerEvent& mouseEvent) {
	auto [worldLocation, valid] = GetMouseLocation(geometry, mouseEvent);
	if (valid) {
		VerticesEdit* verticesEdit = VerticesEdit::GetInstance();
		verticesEdit->OnMouseMove(worldLocation);
	}
}

std::tuple<FVector, bool> AFlyingDefaultPawn::GetMouseLocation(const FGeometry& geometry, const FPointerEvent& mouseEvent) {
	bool valid = false;
	FVector worldLocation = FVector(0,0,0);
	// https://forums.unrealengine.com/t/getting-mouse-location-in-umg/594940
	FVector2D screenPosition = mouseEvent.GetScreenSpacePosition();
	FVector2D localPosition = geometry.AbsoluteToLocal(screenPosition);
	FVector2D localSize = geometry.GetLocalSize();
	// UE_LOG(LogTemp, Display, TEXT("mouse down %s localSize %s localPosition %s"), *screenPosition.ToString(), *localSize.ToString(), *localPosition.ToString());

	// if (GEngine && GEngine->GameViewport) {
	// 	FVector2D ViewportSize = FVector2D(GEngine->GameViewport->Viewport->GetSizeXY());
	// 	UE_LOG(LogTemp, Display, TEXT("viewport %s resolution %f %f"), *ViewportSize.ToString(), GSystemResolution.ResX, GSystemResolution.ResY);
	// }
	// UE_LOG(LogTemp, Display, TEXT("GetAbsolutePosition %s GetAbsoluteSize %s GetLayoutBoundingRect %s GetRenderBoundingRect"),
	// 	*geometry.GetAbsolutePosition().ToString(), *geometry.GetAbsoluteSize().ToString(),
	// 	*geometry.GetLayoutBoundingRect().ToString(), *geometry.GetRenderBoundingRect().ToString());

	UnrealGlobal* unrealGlobal = UnrealGlobal::GetInstance();
	// We know how many meters wide (and tall) the image is (no matter the resolution - it will shrink / stretch to fit).
	// TODO - unhardcode this - use code to figure out widget position / anchor.
	// Widget is half width on right side.
	float widgetXMin = localSize.X / 2;
	float widgetXMax = localSize.X;
	float widgetYMin = 0;
	float widgetYMax = localSize.Y;
	AActor* player = unrealGlobal->GetActorByName("BP_Drone", AFlyingDefaultPawn::StaticClass(), false, true);
	if (player) {
		USceneCaptureComponent2D* captureComponent = player->FindComponentByClass<USceneCaptureComponent2D>();
		float orthographicWidth = captureComponent->OrthoWidth;
		// UE_LOG(LogTemp, Display, TEXT("orthographicWidth %f"), orthographicWidth);

		FVector playerLocation = player->GetActorLocation();
		// Mini map follows player, so player is in center.
		// Local size is width (x size) and height (y size) of mini map (in Unreal units (cm))
		// so can find left, right, top, bottom x and y in world space.
		float xMin = playerLocation.X - orthographicWidth / 2;
		float xMax = playerLocation.X + orthographicWidth / 2;
		float yMin = playerLocation.Y - orthographicWidth / 2;
		float yMax = playerLocation.Y + orthographicWidth / 2;
		// UE_LOG(LogTemp, Display, TEXT("player %s xMin %f xMax %f yMin %f yMax %f"), *playerLocation.ToString(), xMin, xMax, yMin, yMax);

		float x = UKismetMathLibrary::MapRangeClamped(localPosition.X, widgetXMin, widgetXMax, xMin, xMax);
		float y = UKismetMathLibrary::MapRangeClamped(localPosition.Y, widgetYMin, widgetYMax, yMin, yMax);

		// Not sure what the units are; they appear to be meters already??
		// float xMeters = x;
		// float yMeters = y;
		float xMeters = x / unrealGlobal->GetScale();
		float yMeters = y / unrealGlobal->GetScale();
		HeightMap* heightMap = HeightMap::GetInstance();
		float zMeters = heightMap->GetTerrainHeightAtPoint(FVector(xMeters, yMeters, 0));
		worldLocation = FVector(xMeters, yMeters, zMeters);
		valid = true;
	}
	return { worldLocation, valid };
}

// https://forums.unrealengine.com/t/how-to-show-the-mouse-cursor-at-runtime-in-c/35581
void AFlyingDefaultPawn::SetupMouse() {
	APlayerController* playerController = Cast<APlayerController>(GetController());
	if (playerController) {
		playerController->bShowMouseCursor = true;
		playerController->bEnableClickEvents = true; 
		playerController->bEnableMouseOverEvents = true;
	}
}

void AFlyingDefaultPawn::SetupRotation() {
	UnrealGlobal* unrealGlobal = UnrealGlobal::GetInstance();
	AActor* actor = unrealGlobal->GetActorByName("PlayerCameraManager0", APlayerCameraManager::StaticClass(), false, true);
	// TODO - not working - timing?
	if (actor) {
		FVector rotation = FVector(0,0,-90);
		actor->SetActorRotation(FRotator(rotation.Y, rotation.Z, rotation.X));
	} else {
		UE_LOG(LogTemp, Display, TEXT("SetupRotation no actor"));
	}
}

void AFlyingDefaultPawn::SetMiniMapZoom() {
	UnrealGlobal* unrealGlobal = UnrealGlobal::GetInstance();
	AActor* actor = unrealGlobal->GetActorByName("BP_Drone", AFlyingDefaultPawn::StaticClass(), false, true);
	if (actor) {
		USceneCaptureComponent2D* captureComponent = actor->FindComponentByClass<USceneCaptureComponent2D>();
		FVector loc = actor->GetActorLocation();
		float orthoWidth = Lodash::RangeValue(loc.Z, 1000, 50000, 2500, 250000); 
		captureComponent->OrthoWidth = orthoWidth;
		VerticesEdit* verticesEdit = VerticesEdit::GetInstance();
		// TODO - this breaks editing..
		// float scaleX = Lodash::RangeValue(loc.Z, 1000, 50000, 1, 20);
		// verticesEdit->SetDisplayScale(FVector(scaleX, scaleX, scaleX * 1.5), FVector(scaleX / 2, scaleX / 2, scaleX * 1.5 / 2));
	}
}
