// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FlipperMenuHUDWidget.generated.h"

class UTextBlock;
class UButton;
class UVerticalBox;
class UWidgetSwitcher;

UENUM()
enum EFlipperMainMenuPageIndex : uint8
{
	CallToAction = 0,
	FoundSessions = 1
};

/**
 * 
 */
UCLASS(Abstract)
class FLIPPER_API UFlipperMenuHUDWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void SetMenuPageIndex(EFlipperMainMenuPageIndex NewIndex);
	void PopulateResultsPage();

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	
private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWidgetSwitcher> MenuWidgetSwitcher;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVerticalBox> SearchResultsVerticalBox;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> ReturnToCallToActionButton;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> NumberOfSessionsTextBlock;
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUserWidget> SearchResultWidgetClass;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> CloseGameButton;
	
	UFUNCTION()
	void OnReturnToCallToActionButtonPressed();
	
	UFUNCTION()
	void OnCloseGameButtonPressed();

};
