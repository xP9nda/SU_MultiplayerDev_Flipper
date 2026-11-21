// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FlipperMenuFindSessionButtonWidget.generated.h"

class UTextBlock;
class UButton;
/**
 * Bind a button to trigger a find session search, once it's done and complete, send the results back to the HUD and display them
 */
UCLASS(Abstract)
class FLIPPER_API UFlipperMenuFindSessionButtonWidget : public UUserWidget
{
	GENERATED_BODY()

	
	
protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	
private:
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> FindSessionButton;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ButtonTextBlock;

	UFUNCTION()
	void OnFindSessionButtonPressed();
	
	void OnFindSessionsComplete(const TArray<FOnlineSessionSearchResult>& Results, bool bSuccessful);
	
	
};
