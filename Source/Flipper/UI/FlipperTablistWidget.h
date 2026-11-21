// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FlipperTablistWidget.generated.h"

class UButton;
class UFlipperTablistRowWidget;
class UVerticalBox;
/**
 * 
 */
UCLASS(Abstract)
class FLIPPER_API UFlipperTablistWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void ShowTablist();
	void HideTablist();

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	
private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVerticalBox> PlayerRowsVerticalBox;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> LeaveSessionButton; 
	
	UPROPERTY(EditDefaultsOnly, Category = "Flipper|UI")
	TSubclassOf<UFlipperTablistRowWidget> RowWidgetClass;
	
	void RebuildTablist();
	
	UFUNCTION()
	void OnLeaveSessionButtonPressed();
};
