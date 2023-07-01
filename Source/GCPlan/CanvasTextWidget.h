#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"

#include "CanvasTextWidget.generated.h"

UCLASS()
class GCPLAN_API UCanvasTextWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UTextBlock* BottomCenterText;

	UFUNCTION(BlueprintCallable)
	void SetText(FString text);

	UFUNCTION(BlueprintCallable)
	void SetFont(FSlateFontInfo fontInfo);

	UFUNCTION(BlueprintCallable)
	FSlateFontInfo GetFont();
};
