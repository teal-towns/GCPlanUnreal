#pragma once

#include "CoreMinimal.h"
#include "GameFramework/DefaultPawn.h"
#include "Blueprint/UserWidget.h"
#include "EditVerticesWidget.h"

#include "FlyingDefaultPawn.generated.h"

UCLASS()
class GCPLAN_API AFlyingDefaultPawn : public ADefaultPawn
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere)
	UUserWidget* MiniMap;
	// UPROPERTY(EditAnywhere)
	// TSubclassOf<class UEditVerticesWidget> EditVerticesWidgetClass;
	// UPROPERTY()
	// class UEditVerticesWidget* EditVerticesWidget;
	UPROPERTY(EditAnywhere)
	UEditVerticesWidget* EditVerticesWidget;

	// https://forums.unrealengine.com/t/how-to-call-c-function-from-blueprint/453335/2
	// https://forums.unrealengine.com/t/how-do-i-override-onmousebutton-in-c/363249/4
	UFUNCTION(BlueprintCallable)
	// static FEventReply OnMouseUpMiniMap(FGeometry geometry, FPointerEvent& mouseEvent);
	// static void OnMouseDownMiniMap(FGeometry geometry, const FPointerEvent& mouseEvent);
	static void OnMouseDownMiniMap(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent);
	UFUNCTION(BlueprintCallable)
	static void OnMouseUpMiniMap(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent);
	// UFUNCTION(BlueprintCallable)
	// static void OnDragStartMiniMap(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent);
	UFUNCTION(BlueprintCallable)
	static void OnMouseMoveMiniMap(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent);

	static std::tuple<FVector, bool> GetMouseLocation(const FGeometry& geometry, const FPointerEvent& mouseEvent);

	void CreateUI();
	void SetupMouse();
	void SetupRotation();
	void SetMiniMapZoom();
};
