// Fill out your copyright notice in the Description page of Project Settings.


#include "FlipperMenuHUD.h"

#include "Blueprint/UserWidget.h"

void AFlipperMenuHUD::CreateHUD()
{
	APlayerController* OwningController = GetOwningPlayerController();
	if (!OwningController) return;
	
	if (!HUDWidgetClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("FlipperMenuHUD: HudWidgetClass is invalid"));
		return;
	}
	
	HUDWidget = CreateWidget<UUserWidget>(OwningController, HUDWidgetClass);
	if (!HUDWidget) return;
	
	HUDWidget->AddToViewport();
}

void AFlipperMenuHUD::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (HUDWidget)
	{
		HUDWidget->RemoveFromParent();
	}
	
	Super::EndPlay(EndPlayReason);
}
