#include "CanvasTextWidget.h"

#include "Components/CanvasPanelSlot.h"
#include "Fonts/SlateFontInfo.h"

void UCanvasTextWidget::SetText(FString text, FString animateInFunction, int animateOutSeconds,
	FString animateOutFunction, float timePerTextLetter, int lettersPerGroup, bool useBackgroundImage) {
	bool resetTextProperties = true;
	if (animateInFunction.Len() > 0) {
		if (animateInFunction == "ScaleInFadeIn") {
			_animateInDelegate.BindUFunction(this, "ScaleInFadeIn");
			resetTextProperties = false;
			BottomCenterText->SetOpacity(0);
		} else if (animateInFunction == "AnimateTextLetters") {
			_fullText = text;
			_currentText = "";
			_textIndex = 0;
			_timePerTextLetter = timePerTextLetter;
			_lettersPerGroup = lettersPerGroup;
			text = "";
			_animateInDelegate.BindUFunction(this, "AnimateTextLetters");
		} else {
			resetTextProperties = false;
			BottomCenterText->SetOpacity(0);
			_animateInDelegate.BindUFunction(this, "SlideInFadeIn");
		}
		// _animateInDelegate();
		GetWorld()->GetTimerManager().SetTimer(_animateInTimer, _animateInDelegate, 0.01, false);
		if (useBackgroundImage) {
			SetBottomCenterImage(0);
			_animateBottomCenterImageDelegate.BindUFunction(this, "BottomCenterImageFadeIn");
			GetWorld()->GetTimerManager().SetTimer(_animateBottomCenterImageTimer, _animateBottomCenterImageDelegate, 0.01, false);
		}
	}
	if (resetTextProperties) {
		BottomCenterText->SetOpacity(1);
	}
	if (text.Len() < 1) {
		SetBottomCenterImage(0);
	}
	BottomCenterText->SetText(FText::FromString(text));
	if (animateOutSeconds > 0) {
		_animateOutDelegate.BindUFunction(this, "FadeOut");
		GetWorld()->GetTimerManager().SetTimer(_animateOutTimer, _animateOutDelegate, animateOutSeconds, false);
		if (useBackgroundImage) {
			_animateOutBottomCenterImageDelegate.BindUFunction(this, "BottomCenterImageFadeOut");
			GetWorld()->GetTimerManager().SetTimer(_animateOutBottomCenterImageTimer, _animateOutBottomCenterImageDelegate, animateOutSeconds, false);
		}
	}
}

void UCanvasTextWidget::SetFont(FSlateFontInfo fontInfo) {
	BottomCenterText->SetFont(fontInfo);
}

FSlateFontInfo UCanvasTextWidget::GetFont() {
	return BottomCenterText->Font;
}

void UCanvasTextWidget::SetBottomTextPosition(FVector2D position) {
	UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(BottomCenterText->Slot);
	CanvasSlot->SetPosition(position);
}

void UCanvasTextWidget::AnimateTextLetters() {
	if (_textIndex < _fullText.Len() - 1) {
		_textIndex += _lettersPerGroup;
		if (_textIndex > _fullText.Len() - 1) {
			_textIndex = _fullText.Len() - 1;
		}
		_currentText = _fullText.Mid(0, _textIndex + 1);
		SetText(_currentText);
		_animateInDelegate.BindUFunction(this, "AnimateTextLetters");
		GetWorld()->GetTimerManager().SetTimer(_animateInTimer, _animateInDelegate, _timePerTextLetter, false);
	}
}

void UCanvasTextWidget::SetBottomCenterImage(float opacity, float sizeY) {
	BottomCenterImage->SetOpacity(opacity);
	UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(BottomCenterText->Slot);
	FVector2D size = CanvasSlot->GetSize();
	CanvasSlot->SetSize(FVector2D(size.X, sizeY));
}

void UCanvasTextWidget::SetImageAnimate(FString animateFunction) {
	if (animateFunction == "BackgroundImageFadeIn") {
		_animateImageDelegate.BindUFunction(this, "BackgroundImageFadeIn");
	} else {
		_animateImageDelegate.BindUFunction(this, "BackgroundImageFadeOut");
	}
	GetWorld()->GetTimerManager().SetTimer(_animateImageTimer, _animateImageDelegate, 0.01, false);
}

void UCanvasTextWidget::SetImage(float opacity) {
	BackgroundImage->SetOpacity(opacity);
}

void UCanvasTextWidget::PlayMovie() {
	MovieImage->SetVisibility(ESlateVisibility::Visible);
	_animateMovieDelegate.BindUFunction(this, "MovieImagePlay");
	GetWorld()->GetTimerManager().SetTimer(_animateMovieTimer, _animateMovieDelegate, 0.01, false);
}

void UCanvasTextWidget::HideMovie() {
	MovieImage->SetVisibility(ESlateVisibility::Hidden);
}