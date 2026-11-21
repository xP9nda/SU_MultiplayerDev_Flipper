// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FlipperMenuStartSessionButtonWidget.generated.h"

class UTextBlock;
class UButton;
/**
 * Binds a button to create, then start a session
 */
UCLASS(Abstract)
class FLIPPER_API UFlipperMenuStartSessionButtonWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> StartSessionButton;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ButtonTextBlock;
	
	UFUNCTION()
	void OnStartSessionComplete(bool bSuccessful);
	UFUNCTION()
	void OnCreateSessionComplete(bool bSuccessful);
	
	UFUNCTION()
	void OnStartSessionButtonPressed();
	
};
