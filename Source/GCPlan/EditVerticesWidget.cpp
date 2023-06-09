#include "EditVerticesWidget.h"

#include "Landscape/VerticesEdit.h"

void UEditVerticesWidget::Init() {
	// https://forums.unrealengine.com/t/add-onclick-action-c/384233
	PlusButton->OnClicked.AddDynamic(this, &UEditVerticesWidget::PressPlus);
	TrashButton->OnClicked.AddDynamic(this, &UEditVerticesWidget::PressTrash);
	TypeSelect->OnSelectionChanged.AddDynamic(this, &UEditVerticesWidget::SelectType);
	// SetPlusActive(true);
}

void UEditVerticesWidget::PressPlus() {
	SetPlusActive(!_plusActive);
}

void UEditVerticesWidget::SetPlusActive(bool active) {
	_plusActive = active;
	VerticesEdit* verticesEdit = VerticesEdit::GetInstance();
	FString mode = _plusActive ? "add" : "";
	verticesEdit->SetMode(mode);
	FLinearColor color = _plusActive ? FLinearColor(0,150,0,1) : FLinearColor(255,255,255,1);
	PlusButton->SetColorAndOpacity(color);
}

void UEditVerticesWidget::PressTrash() {
	VerticesEdit* verticesEdit = VerticesEdit::GetInstance();
	verticesEdit->Trash();
}

void UEditVerticesWidget::SelectType(FString sItem, ESelectInfo::Type seltype) {
	TMap<int, FString> optionsMap = {
		{ 0, "plot" },
		{ 1, "building" },
		{ 2, "road" }
	};
	VerticesEdit* verticesEdit = VerticesEdit::GetInstance();
	verticesEdit->SetType(optionsMap[TypeSelect->GetSelectedIndex()]);
}
