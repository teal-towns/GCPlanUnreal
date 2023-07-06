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
	void SetText(FString text, FString animateInFunction = "",
		int animateOutSeconds = -1, FString animateOutFunction = "",
		float timePerTextLetter = 0.01);
	UFUNCTION(BlueprintCallable)
	void SetFont(FSlateFontInfo fontInfo);
	UFUNCTION(BlueprintCallable)
	FSlateFontInfo GetFont();
	UFUNCTION(BlueprintCallable)
	void AnimateTextLetters();

	UFUNCTION(BlueprintImplementableEvent)
	void FadeOut();
	UFUNCTION(BlueprintImplementableEvent)
	void ScaleInFadeIn();
	UFUNCTION(BlueprintImplementableEvent)
	void SlideInFadeIn();

private:
	FTimerHandle _animateOutTimer;
	FTimerDelegate _animateOutDelegate;

	FTimerHandle _animateInTimer;
	FTimerDelegate _animateInDelegate;

	float _timePerTextLetter = 0.01;
	FString _fullText = "";
	int _textIndex = 0;
	FString _currentText = "";
};
