// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "FlipperHUD.generated.h"

class UFlipperTablistWidget;
class UUserWidget;

/**
 * 
 */
UCLASS(Abstract)
class FLIPPER_API AFlipperHUD : public AHUD
{
	GENERATED_BODY()

public:
	void CreateHUD();
	void SetTablistVisible(bool bIsVisible);
	
protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY(EditDefaultsOnly, Category = "Flipper|HUD")
	TSubclassOf<UUserWidget> HUDWidgetClass; 
	
	UPROPERTY(EditDefaultsOnly, Category = "Flipper|HUD")
	TSubclassOf<UFlipperTablistWidget> TablistWidgetClass;
	
private:
	UPROPERTY()
	TObjectPtr<UUserWidget> HUDWidget;
	
	UPROPERTY()
	TObjectPtr<UFlipperTablistWidget> TablistWidget;
};
