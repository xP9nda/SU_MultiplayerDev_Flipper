// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Blueprint/UserWidget.h"
#include "Components/FlipperUsableUnitComponent.h"
#include "FlipperUnitsWidget.generated.h"

class UProgressBar;
class UImage;
class UTextBlock;
/**
 * 
 */
UCLASS(Abstract)
class FLIPPER_API UFlipperUnitsWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void NativePreConstruct() override;
	
	UFUNCTION()
	void HandlePossessedPawnChanged(APawn* NewPawn, APawn* OldPawn);
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Flipper|Units", meta = (Categories = "Units"))
	FGameplayTag UnitType;
	
	UPROPERTY(BlueprintReadOnly, Category = "Flipper|Units")
	TObjectPtr<UFlipperUsableUnitComponent> ObservedUnitComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Flipper|UI")
	TObjectPtr<UTexture2D> UnitIconTexture;
	
	UPROPERTY(meta = (BindWidget="UnitIcon"))
	TObjectPtr<UImage> UnitIcon;
	
	UPROPERTY(meta = (BindWidget="MaxUnitsAvailableText"))
	TObjectPtr<UTextBlock> MaxUnitsAvailableText;
	
	UPROPERTY(meta = (BindWidget="CurrentUnitsAvailableText"))
	TObjectPtr<UTextBlock> CurrentUnitsAvailableText;
	
	UPROPERTY(meta = (BindWidget="CurrentFinishDisplayNameText"))
	TObjectPtr<UTextBlock> CurrentFinishDisplayNameText;
	
	UPROPERTY(meta = (BindWidget="UnitsProgressBar"))
	TObjectPtr<UProgressBar> UnitsProgressBar;
	
private:
	void UpdateVisibility();
	
	UFUNCTION()
	void UpdateDisplay();
};
