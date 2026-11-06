// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FlipperTeamFundsWidget.generated.h"

class UTextBlock;
class AFlipperGameState;
/**
 * 
 */
UCLASS(Abstract)
class FLIPPER_API UFlipperTeamFundsWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	
private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TeamFundsText;
	
	TWeakObjectPtr<AFlipperGameState> ObservedGameState;
	
	UFUNCTION()
	void HandleTeamFundsChanged(int32 NewFunds);
};
