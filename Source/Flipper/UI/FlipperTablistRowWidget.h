// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Blueprint/UserWidget.h"
#include "FlipperTablistRowWidget.generated.h"

class UBorder;
class UTextBlock;
class AFlipperPlayerState;
/**
 * 
 */
UCLASS(Abstract)
class FLIPPER_API UFlipperTablistRowWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void Setup(AFlipperPlayerState* PlayerState);

	// PostSetup is called at the end of Setup, before RefreshVisuals
	UFUNCTION(BlueprintImplementableEvent)
	void PostSetup();
	
protected:
	virtual void NativeDestruct() override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flipper|UI", meta = (Categories = "Units"))
	TMap<FGameplayTag, UTextBlock*> PickupUnitTypeTextBlocks;
	
private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> PlayerNameTextBlock;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> CorrectStepsTextBlock;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> IncorrectStepsTextBlock;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> FundsEarnedTextBlock;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> FundsSpentTextBlock;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UBorder> SelfHighlightBorder;
	
	TWeakObjectPtr<AFlipperPlayerState> ObservedPlayerState;
	
	UFUNCTION()
	void OnContributionChanged();
	
	void RefreshVisuals();
};
