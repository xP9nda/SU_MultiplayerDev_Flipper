// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Blueprint/UserWidget.h"
#include "FlipperUnitsWidget.generated.h"

class UFlipperUsableUnitComponent;
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
	virtual void NativeDestruct() override;

	UFUNCTION()
	void HandlePossessedPawnChanged(APawn* OldPawn, APawn* NewPawn);
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Flipper|Units", meta = (Categories = "Units"))
	FGameplayTag UnitType;
	
	UPROPERTY(BlueprintReadOnly, Category = "Flipper|Units")
	TObjectPtr<UFlipperUsableUnitComponent> ObservedUnitComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Flipper|UI")
	TObjectPtr<UTexture2D> UnitIconTexture;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> UnitIcon;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> MaxUnitsAvailableText;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> CurrentUnitsAvailableText;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> CurrentFinishDisplayNameText;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> UnitsProgressBar;
	
private:
	void ObserveComponent(UFlipperUsableUnitComponent* NewComponent);
	void UpdateVisibility();
	
	UFUNCTION()
	void UpdateDisplay();
};
