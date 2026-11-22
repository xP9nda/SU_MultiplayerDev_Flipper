// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FlipperMenuSessionErrorWidget.generated.h"

class UTextBlock;
class UButton;
/**
 * 
 */
UCLASS(Abstract)
class FLIPPER_API UFlipperMenuSessionErrorWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

private:
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ErrorTextBlock;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> ConfirmButton;
	
	UFUNCTION()
	void OnConfirmButtonPressed();
};
