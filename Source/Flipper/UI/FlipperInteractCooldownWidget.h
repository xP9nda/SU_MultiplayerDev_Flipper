// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FlipperInteractCooldownWidget.generated.h"

class UImage;
class UFlipperInteractComponent;
/**
 * 
 */
UCLASS(Abstract)
class FLIPPER_API UFlipperInteractCooldownWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	virtual void NativeDestruct() override;

	UFUNCTION()
	void HandlePossessedPawnChanged(APawn* OldPawn, APawn* NewPawn);
	
	UPROPERTY(BlueprintReadOnly, Category = "Flipper|Interaction")
	TObjectPtr<UFlipperInteractComponent> ObservedInteractComponent;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> InteractProgressImage;
	
	UPROPERTY(EditDefaultsOnly, Category = "Flipper|UI")
	TObjectPtr<UMaterialInstance> InteractProgressMaterial;
	
	UPROPERTY(EditDefaultsOnly, Category = "Flipper|UI")
	FName MaterialFillProgressParameterName = FName("FillProgress");
	
private:
	TWeakObjectPtr<UMaterialInstanceDynamic> InteractProgressMaterialDynamic;

	bool bIsAnimating = false;
	double AnimationEndTime = 0.0;
	double AnimationDuration;
	
	UFUNCTION()
	void HandleOnInteract();
	void ObserveInteractComponent(UFlipperInteractComponent* NewComponent);
	
};
