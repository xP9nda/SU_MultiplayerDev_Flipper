// Fill out your copyright notice in the Description page of Project Settings.


#include "FlipperHUD.h"

#include "Blueprint/UserWidget.h"

void AFlipperHUD::BeginPlay()
{
	Super::BeginPlay();
	
	APlayerController* OwningController = GetOwningPlayerController();
	if (!OwningController) return;
	
	if (!HUDWidgetClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("FlipperHUD: HudWidgetClass is invalid"));
		return;
	}
	
	HUDWidget = CreateWidget<UUserWidget>(OwningController, HUDWidgetClass);
	if (!HUDWidget) return;
	
	HUDWidget->AddToViewport();
}

void AFlipperHUD::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	
	if (!HUDWidget) return;
	
	HUDWidget->RemoveFromParent();
}
