// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Blueprint/UserWidget.h"
#include "FlipperUnitsWidget.generated.h"

class UFlipperToolComponent;
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

	UPROPERTY(BlueprintReadOnly, Category = "Flipper|Units")
	TObjectPtr<UFlipperUsableUnitComponent> ObservedUnitComponent;
	
	UPROPERTY(BlueprintReadOnly, Category = "Flipper|Units")
	TObjectPtr<UFlipperToolComponent> ObservedToolComponent;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> MaxUnitsAvailableText;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> CurrentUnitsAvailableText;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> CurrentFinishDisplayNameText;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> CurrentToolDisplayNameText;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> UnitsProgressImage;
	
	UPROPERTY(EditDefaultsOnly, Category = "Flipper|UI")
	TObjectPtr<UMaterialInstance> UnitsProgressMaterial;
	
	UPROPERTY(EditDefaultsOnly, Category = "Flipper|UI")
	FName MaterialFillProgressParameterName = FName("Fill amount");
	
	UPROPERTY(EditDefaultsOnly, Category = "Flipper|UI")
	FName MaterialColourParameterName = FName("Liquid Color");
	
private:
	TWeakObjectPtr<UMaterialInstanceDynamic> UnitsProgressMaterialDynamic;
	
	void ObserveUnitsComponent(UFlipperUsableUnitComponent* NewComponent);
	void ObserveToolComponent(UFlipperToolComponent* NewComponent);
	
	UFUNCTION()
	void HandleToolChanged(FGameplayTag NewTool);

	void ClearDisplay();
	UFUNCTION()
	void UpdateDisplay();
};
