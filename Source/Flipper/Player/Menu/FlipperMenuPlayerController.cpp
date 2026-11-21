// Fill out your copyright notice in the Description page of Project Settings.


#include "FlipperMenuPlayerController.h"

#include "FlipperMenuHUD.h"

void AFlipperMenuPlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	// set input modes
	FInputModeUIOnly InputMode;
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	
	SetShowMouseCursor(true);
	SetInputMode(InputMode);
	
	// create UI
	AFlipperMenuHUD* MenuHUD = GetHUD<AFlipperMenuHUD>();
	if (!MenuHUD) return;
	
	MenuHUD->CreateHUD();
}
