#include "CanvasTextWidget.h"

#include "Fonts/SlateFontInfo.h"

void UCanvasTextWidget::SetText(FString text) {
	BottomCenterText->SetText(FText::FromString(text));
}

void UCanvasTextWidget::SetFont(FSlateFontInfo fontInfo) {
	BottomCenterText->SetFont(fontInfo);
}

FSlateFontInfo UCanvasTextWidget::GetFont() {
	return BottomCenterText->Font;
}