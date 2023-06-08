#pragma once

#include "CoreMinimal.h"
#include "GameFramework/DefaultPawn.h"
#include "Blueprint/UserWidget.h"

#include "FlyingDefaultPawn.generated.h"

UCLASS()
class GCPLAN_API AFlyingDefaultPawn : public ADefaultPawn
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

public:
	UPROPERTY(EditAnywhere)
	UUserWidget* MiniMap;

	// https://forums.unrealengine.com/t/how-to-call-c-function-from-blueprint/453335/2
	// https://forums.unrealengine.com/t/how-do-i-override-onmousebutton-in-c/363249/4
	UFUNCTION(BlueprintCallable)
	// static FEventReply OnMouseUpMiniMap(FGeometry geometry, FPointerEvent& mouseEvent);
	// static void OnMouseDownMiniMap(FGeometry geometry, const FPointerEvent& mouseEvent);
	static void OnMouseDownMiniMap(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent);

	void CreateMiniMap();
	void SetupMouse();
};
