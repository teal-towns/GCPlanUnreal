#include "CanvasTextWidget.h"

#include "Fonts/SlateFontInfo.h"

void UCanvasTextWidget::SetText(FString text, FString animateInFunction, int animateOutSeconds,
	FString animateOutFunction, float timePerTextLetter, int lettersPerGroup) {
	bool resetTextProperties = true;
	if (animateInFunction.Len() > 0) {
		if (animateInFunction == "ScaleInFadeIn") {
			_animateInDelegate.BindUFunction(this, "ScaleInFadeIn");
			resetTextProperties = false;
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
			_animateInDelegate.BindUFunction(this, "SlideInFadeIn");
		}
		// _animateInDelegate();
		GetWorld()->GetTimerManager().SetTimer(_animateInTimer, _animateInDelegate, 0.01, false);
	}
	if (resetTextProperties) {
		BottomCenterText->SetOpacity(1);
	}
	BottomCenterText->SetText(FText::FromString(text));
	if (animateOutSeconds > 0) {
		_animateOutDelegate.BindUFunction(this, "FadeOut");
		GetWorld()->GetTimerManager().SetTimer(_animateOutTimer, _animateOutDelegate, animateOutSeconds, false);
	}
}

void UCanvasTextWidget::SetFont(FSlateFontInfo fontInfo) {
	BottomCenterText->SetFont(fontInfo);
}

FSlateFontInfo UCanvasTextWidget::GetFont() {
	return BottomCenterText->Font;
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