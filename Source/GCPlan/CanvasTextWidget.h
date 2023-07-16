#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

#include "CanvasTextWidget.generated.h"

UCLASS()
class GCPLAN_API UCanvasTextWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UTextBlock* BottomCenterText;
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UImage* BottomCenterImage;
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UImage* BackgroundImage;
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UImage* MovieImage;

	UFUNCTION(BlueprintCallable)
	void SetText(FString text, FString animateInFunction = "",
		int animateOutSeconds = -1, FString animateOutFunction = "",
		float timePerTextLetter = 0.01, int lettersPerGroup = 1,
		bool useBackgroundImage = false);
	UFUNCTION(BlueprintCallable)
	void SetFont(FSlateFontInfo fontInfo);
	UFUNCTION(BlueprintCallable)
	FSlateFontInfo GetFont();
	void SetBottomTextPosition(FVector2D position);
	UFUNCTION(BlueprintCallable)
	void AnimateTextLetters();

	void SetBottomCenterImage(float opacity, float sizeY = 250);
	UFUNCTION(BlueprintImplementableEvent)
	void BottomCenterImageFadeIn();
	UFUNCTION(BlueprintImplementableEvent)
	void BottomCenterImageFadeOut();

	UFUNCTION(BlueprintImplementableEvent)
	void FadeOut();
	UFUNCTION(BlueprintImplementableEvent)
	void ScaleInFadeIn();
	UFUNCTION(BlueprintImplementableEvent)
	void SlideInFadeIn();

	void SetImageAnimate(FString animationFunction = "BackgroundImageFadeIn");
	void SetImage(float opacity);

	UFUNCTION(BlueprintImplementableEvent)
	void BackgroundImageFadeIn();
	UFUNCTION(BlueprintImplementableEvent)
	void BackgroundImageFadeOut();

	UFUNCTION(BlueprintImplementableEvent)
	void MovieImagePlay();
	// UFUNCTION(BlueprintImplementableEvent)
	// void MovieImageStop();

	void PlayMovie();
	void HideMovie();

private:
	FTimerHandle _animateOutTimer;
	FTimerDelegate _animateOutDelegate;

	FTimerHandle _animateInTimer;
	FTimerDelegate _animateInDelegate;

	float _timePerTextLetter = 0.01;
	int _lettersPerGroup = 1;
	FString _fullText = "";
	int _textIndex = 0;
	FString _currentText = "";

	FTimerHandle _animateImageTimer;
	FTimerDelegate _animateImageDelegate;

	FTimerHandle _animateMovieTimer;
	FTimerDelegate _animateMovieDelegate;

	FTimerHandle _animateBottomCenterImageTimer;
	FTimerDelegate _animateBottomCenterImageDelegate;
	FTimerHandle _animateOutBottomCenterImageTimer;
	FTimerDelegate _animateOutBottomCenterImageDelegate;
};
