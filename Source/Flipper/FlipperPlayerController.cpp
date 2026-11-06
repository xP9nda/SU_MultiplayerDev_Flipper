// Copyright Epic Games, Inc. All Rights Reserved.


#include "FlipperPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "InputMappingContext.h"
#include "FlipperCameraManager.h"
#include "Blueprint/UserWidget.h"
#include "Flipper.h"
#include "Cheats/FlipperCheatManager.h"
#include "Core/FlipperGameState.h"
#include "GameFramework/GameMode.h"
#include "Widgets/Input/SVirtualJoystick.h"


AFlipperPlayerController::AFlipperPlayerController()
{
	// set the player camera manager class
	PlayerCameraManagerClass = AFlipperCameraManager::StaticClass();
	
	CheatClass = UFlipperCheatManager::StaticClass();
}

void AFlipperPlayerController::Server_Cheat_AddFunds_Implementation(int32 Amount)
{
#if !UE_BUILD_SHIPPING
	
	UWorld* World = GetWorld();
	if (!World) return;
	
	AGameModeBase* AuthGameMode = World->GetAuthGameMode();
	if (!AuthGameMode) return;
	if (!AuthGameMode->AllowCheats(this)) return;
	
	AFlipperGameState* GameState = Cast<AFlipperGameState>(World->GetGameState());
	if (!GameState) return;
	
	GameState->AddFunds(Amount);
	
#endif
}

void AFlipperPlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	// only spawn touch controls on local player controllers
	if (IsLocalPlayerController() && ShouldUseTouchControls())
	{
		// spawn the mobile controls widget
		MobileControlsWidget = CreateWidget<UUserWidget>(this, MobileControlsWidgetClass);

		if (MobileControlsWidget)
		{
			// add the controls to the player screen
			MobileControlsWidget->AddToPlayerScreen(0);

		} else {

			UE_LOG(LogFlipper, Error, TEXT("Could not spawn mobile controls widget."));

		}
	}
	
	// set input modes
	SetInputMode(FInputModeGameOnly());
	bShowMouseCursor = false;
}

void AFlipperPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// only add IMCs for local player controllers
	if (IsLocalPlayerController())
	{
		// Add Input Mapping Context
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
		{
			for (UInputMappingContext* CurrentContext : DefaultMappingContexts)
			{
				Subsystem->AddMappingContext(CurrentContext, 0);
			}

			// only add these IMCs if we're not using mobile touch input
			if (!ShouldUseTouchControls())
			{
				for (UInputMappingContext* CurrentContext : MobileExcludedMappingContexts)
				{
					Subsystem->AddMappingContext(CurrentContext, 0);
				}
			}
		}
	}
	
}

bool AFlipperPlayerController::ShouldUseTouchControls() const
{
	// are we on a mobile platform? Should we force touch?
	return SVirtualJoystick::ShouldDisplayTouchInterface() || bForceTouchControls;
}
