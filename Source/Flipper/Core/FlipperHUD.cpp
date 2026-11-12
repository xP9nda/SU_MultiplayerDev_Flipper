// Fill out your copyright notice in the Description page of Project Settings.


#include "FlipperHUD.h"

#include "Blueprint/UserWidget.h"
#include "UI/FlipperTablistWidget.h"

void AFlipperHUD::BeginPlay()
{
	Super::BeginPlay();
}

void AFlipperHUD::CreateHUD()
{
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

void AFlipperHUD::SetTablistVisible(bool bIsVisible)
{
	// if the tablist widget does not exist, create it
	if (!TablistWidget)
	{
		if (!bIsVisible || !TablistWidgetClass) return;
		
		TablistWidget = CreateWidget<UFlipperTablistWidget>(GetOwningPlayerController(), TablistWidgetClass);
		if (!TablistWidget) return;
		
		TablistWidget->AddToViewport(99); // add the tablist on top of everything else
	}
	
	bIsVisible ? TablistWidget->ShowTablist() : TablistWidget->HideTablist();
	
	// ensure the tablist is centered and doesn't fill the screen
	TablistWidget->SetAnchorsInViewport(FAnchors(0.5f, 0.5f));
	TablistWidget->SetAlignmentInViewport(FVector2D(0.5f, 0.5f));
}

void AFlipperHUD::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	
	if (TablistWidget)
	{
		TablistWidget->RemoveFromParent();
	}
	
	if (HUDWidget)
	{
		HUDWidget->RemoveFromParent();
	}
}
