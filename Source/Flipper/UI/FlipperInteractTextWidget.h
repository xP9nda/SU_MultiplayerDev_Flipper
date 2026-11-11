// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FlipperInteractTextWidget.generated.h"

class UTextBlock;
class UFlipperInteractComponent;

/**
 * 
 */
UCLASS(Abstract)
class FLIPPER_API UFlipperInteractTextWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	
private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> InteractMainText;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> InteractDescriptionText;
	
	// how often the text is updated when looking at something, as state can change without looking away
	// (for example, after selling units at the unit seller)
	UPROPERTY(EditDefaultsOnly, Category = "Flipper|UI")
	float RefreshInterval = 0.1f;
	
	void ObserveComponent(UFlipperInteractComponent* NewComponent);
	void SetWidgetVisibility(bool bVisible);

	UFUNCTION()
	void HandleLookedAtActorChanged(AActor* NewLookedAtActor);
	void RefreshInteractText();

	UPROPERTY()
	TObjectPtr<UFlipperInteractComponent> ObservedComponent;
	
	TWeakObjectPtr<AActor> LookedAtActor;
	FTimerHandle RefreshTimerHandle;
};
