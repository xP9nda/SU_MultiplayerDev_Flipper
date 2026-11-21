// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "FlipperMenuHUD.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class FLIPPER_API AFlipperMenuHUD : public AHUD
{
	GENERATED_BODY()
	
public:
	void CreateHUD();
	
	UUserWidget* GetHUDWidget() const { return HUDWidget; }
	template<typename T>
	T* GetHUDWidget() const { return Cast<T>(HUDWidget); }

protected:
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	UPROPERTY(EditDefaultsOnly, Category = "Flipper|HUD")
	TSubclassOf<UUserWidget> HUDWidgetClass; 
	
private:
	UPROPERTY()
	TObjectPtr<UUserWidget> HUDWidget;
};
