// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FlipperInteractCrosshairWidget.generated.h"

class UImage;
class UFlipperInteractComponent;
/**
 * 
 */
UCLASS(Abstract)
class FLIPPER_API UFlipperInteractCrosshairWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	virtual void NativeDestruct() override;

	UFUNCTION()
	void HandlePossessedPawnChanged(APawn* OldPawn, APawn* NewPawn);
	
	UPROPERTY(BlueprintReadOnly, Category = "Flipper|Interaction")
	TObjectPtr<UFlipperInteractComponent> ObservedInteractComponent;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> InteractProgressImage;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> CrosshairImage;
	
	UPROPERTY(EditDefaultsOnly, Category = "Flipper|UI")
	TObjectPtr<UMaterialInstance> InteractProgressMaterial;
	
	UPROPERTY(EditDefaultsOnly, Category = "Flipper|UI")
	FName MaterialFillProgressParameterName = FName("FillProgress");
	
	UPROPERTY(EditDefaultsOnly, Category = "Flipper|UI")
	FSlateBrush DefaultCrosshairBrush;
	
	UPROPERTY(EditDefaultsOnly, Category = "Flipper|UI")
	FSlateBrush HoverCrosshairBrush;
	
private:
	TWeakObjectPtr<UMaterialInstanceDynamic> InteractProgressMaterialDynamic;

	bool bIsAnimating = false;
	double AnimationEndTime = 0.0;
	double AnimationDuration;
	
	UFUNCTION()
	void HandleOnInteract();
	
	UFUNCTION()
	void HandleOnLookedAtActorChanged(AActor* LookedAtActor);
	
	void ObserveInteractComponent(UFlipperInteractComponent* NewComponent);

protected:
};
