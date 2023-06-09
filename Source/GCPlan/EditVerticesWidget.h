#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Components/ComboBoxString.h"
#include "EditVerticesWidget.generated.h"

UCLASS()
class GCPLAN_API UEditVerticesWidget : public UUserWidget
{
	GENERATED_BODY()

private:
	bool _plusActive = false;

public:
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UComboBoxString* TypeSelect;
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UButton* PlusButton;
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UButton* TrashButton;

	UFUNCTION(BlueprintCallable)
	void PressPlus();
	UFUNCTION(BlueprintCallable)
	void PressTrash();
	UFUNCTION(BlueprintCallable)
	void SelectType(FString sItem, ESelectInfo::Type seltype);

	void SetPlusActive(bool active);

	void Init();
};
