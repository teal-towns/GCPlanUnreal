#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Components/ComboBoxString.h"
#include "Components/EditableTextBox.h"
#include "Components/MultiLineEditableTextBox.h"
#include "EditVerticesWidget.generated.h"

UCLASS()
class GCPLAN_API UEditVerticesWidget : public UUserWidget
{
	GENERATED_BODY()

private:
	bool _plusActive = false;
	double _debounceSeconds = 1;
	FTimerHandle TypeTimer;
	FTimerDelegate _delegateType;
	FTimerHandle TagsTimer;
	FTimerDelegate _delegateTags;
	FTimerHandle FilterTypeTimer;
	FTimerDelegate _delegateFilterType;

public:
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UComboBoxString* ShapeSelect;
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UButton* PlusButton;
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UButton* TrashButton;
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UEditableTextBox* TypeText;
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UMultiLineEditableTextBox* TagsText;
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UEditableTextBox* FilterTypeText;

	UFUNCTION(BlueprintCallable)
	void PressPlus();
	UFUNCTION(BlueprintCallable)
	void PressTrash();
	UFUNCTION(BlueprintCallable)
	void SelectShape(FString sItem, ESelectInfo::Type seltype);
	UFUNCTION(BlueprintCallable)
	void ChangeType(const FText& text);
	UFUNCTION(BlueprintCallable)
	void ChangeTagsString(const FText& text);
	UFUNCTION(BlueprintCallable)
	void ChangeFilterType(const FText& text);

	UFUNCTION()
	void ChangeTypeActual(const FText& text);
	UFUNCTION()
	void ChangeTagsStringActual(const FText& text);
	UFUNCTION()
	void ChangeFilterTypeActual(const FText& text);

	void SetPlusActive(bool active);

	void Init();
};
