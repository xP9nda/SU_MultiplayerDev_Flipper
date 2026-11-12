// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FlipperTablistWidget.generated.h"

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
	
private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVerticalBox> PlayerRowsVerticalBox;
	
	UPROPERTY(EditDefaultsOnly, Category = "Flipper|UI")
	TSubclassOf<UFlipperTablistRowWidget> RowWidgetClass;
	
	void RebuildTablist();
};
