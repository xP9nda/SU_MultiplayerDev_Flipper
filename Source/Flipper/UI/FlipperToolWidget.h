// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Blueprint/UserWidget.h"
#include "FlipperToolWidget.generated.h"

class UFlipperToolComponent;
class UImage;
/**
 * 
 */
UCLASS(Abstract)
class FLIPPER_API UFlipperToolWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	
protected:
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	
	UFUNCTION()
	void HandlePossessedPawnChanged(APawn* OldPawn, APawn* NewPawn);
	
	UFUNCTION(BlueprintImplementableEvent, Category = "Flipper|Tools")
	void OnSelectedToolChanged(bool bIsToolSelected);
	
private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> ToolImageWidget;
	
	UPROPERTY(EditAnywhere, Category = "Flipper|UI")
	TObjectPtr<UTexture2D> ToolImageTexture;
	
	UPROPERTY(EditAnywhere, Category = "Flipper|Units", meta = (Categories = "Units"))
	FGameplayTag ToolUnitType;
	
	UPROPERTY()
	TObjectPtr<UFlipperToolComponent> ObservedToolComponent;
	
	void ObserveComponent(UFlipperToolComponent* NewComponent);
	
	UFUNCTION()
	void HandleToolChanged(FGameplayTag NewTool);
};
