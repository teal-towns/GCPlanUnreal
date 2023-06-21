#include "EditVerticesWidget.h"

#include "Landscape/VerticesEdit.h"

void UEditVerticesWidget::Init() {
	// https://forums.unrealengine.com/t/add-onclick-action-c/384233
	PlusButton->OnClicked.AddDynamic(this, &UEditVerticesWidget::PressPlus);
	TrashButton->OnClicked.AddDynamic(this, &UEditVerticesWidget::PressTrash);
	ShapeSelect->OnSelectionChanged.AddDynamic(this, &UEditVerticesWidget::SelectShape);
	TypeText->OnTextChanged.AddDynamic(this, &UEditVerticesWidget::ChangeType);
	TagsText->OnTextChanged.AddDynamic(this, &UEditVerticesWidget::ChangeTagsString);
	FilterTypeText->OnTextChanged.AddDynamic(this, &UEditVerticesWidget::ChangeFilterType);
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
	FLinearColor color = _plusActive ? FLinearColor(0,150/255,0,1) : FLinearColor(1,1,1,1);
	PlusButton->SetColorAndOpacity(color);
}

void UEditVerticesWidget::PressTrash() {
	VerticesEdit* verticesEdit = VerticesEdit::GetInstance();
	verticesEdit->Trash();
}

void UEditVerticesWidget::SelectShape(FString sItem, ESelectInfo::Type seltype) {
	// Hardcoded must match UI.
	TMap<int, FString> optionsMap = {
		{ 0, "polygon" },
		{ 1, "path" },
		{ 2, "point" }
	};
	VerticesEdit* verticesEdit = VerticesEdit::GetInstance();
	verticesEdit->SetShape(optionsMap[ShapeSelect->GetSelectedIndex()]);
}

void UEditVerticesWidget::ChangeType(const FText& text) {
	GetWorld()->GetTimerManager().ClearTimer(TypeTimer);
	_delegateType.BindUFunction(this, "ChangeTypeActual", text);
	GetWorld()->GetTimerManager().SetTimer(TypeTimer, _delegateType, _debounceSeconds, false);
}

void UEditVerticesWidget::ChangeTagsString(const FText& text) {
	GetWorld()->GetTimerManager().ClearTimer(TagsTimer);
	_delegateTags.BindUFunction(this, "ChangeTagsStringActual", text);
	GetWorld()->GetTimerManager().SetTimer(TagsTimer, _delegateTags, _debounceSeconds, false);
	// VerticesEdit* verticesEdit = VerticesEdit::GetInstance();
	// TArray<FString> tags;
	// text.ToString().ParseIntoArray(tags, TEXT(","), true);
	// verticesEdit->SetTags(tags);
}

void UEditVerticesWidget::ChangeFilterType(const FText& text) {
	GetWorld()->GetTimerManager().ClearTimer(FilterTypeTimer);
	_delegateFilterType.BindUFunction(this, "ChangeFilterTypeActual", text);
	GetWorld()->GetTimerManager().SetTimer(FilterTypeTimer, _delegateFilterType, _debounceSeconds, false);
}

void UEditVerticesWidget::ChangeTypeActual(const FText& text) {
	VerticesEdit* verticesEdit = VerticesEdit::GetInstance();
	verticesEdit->SetType(text.ToString());
}

void UEditVerticesWidget::ChangeTagsStringActual(const FText& text) {
	VerticesEdit* verticesEdit = VerticesEdit::GetInstance();
	// TArray<FString> tags;
	// text.ToString().ParseIntoArray(tags, TEXT(","), true);
	verticesEdit->SetPairsString(text.ToString());
}

void UEditVerticesWidget::ChangeFilterTypeActual(const FText& text) {
	VerticesEdit* verticesEdit = VerticesEdit::GetInstance();
	TArray<FString> types;
	text.ToString().ParseIntoArray(types, TEXT(","), true);
	verticesEdit->SetFilterTypes(types);
}
