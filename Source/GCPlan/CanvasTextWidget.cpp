#include "CanvasTextWidget.h"

void UCanvasTextWidget::SetText(FString text) {
	BottomCenterText->SetText(FText::FromString(text));
}