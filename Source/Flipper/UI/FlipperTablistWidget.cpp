// Fill out your copyright notice in the Description page of Project Settings.


#include "FlipperTablistWidget.h"

#include "FlipperTablistRowWidget.h"
#include "Components/Button.h"
#include "Components/VerticalBox.h"
#include "Core/FlipperGameInstance.h"
#include "Core/FlipperPlayerState.h"
#include "GameFramework/GameStateBase.h"
#include "Subsystems/FlipperSessionSubsystem.h"

void UFlipperTablistWidget::ShowTablist()
{
	RebuildTablist();
	SetVisibility(ESlateVisibility::Visible); // visible so entire tablist can receive input for button actions
	
	APlayerController* OwningPlayer = GetOwningPlayer();
	if (OwningPlayer)
	{
		OwningPlayer->SetShowMouseCursor(true);
		OwningPlayer->SetInputMode(FInputModeGameAndUI());
	}
}

void UFlipperTablistWidget::HideTablist()
{
	SetVisibility(ESlateVisibility::Collapsed);
	PlayerRowsVerticalBox->ClearChildren();
	
	APlayerController* OwningPlayer = GetOwningPlayer();
	if (OwningPlayer)
	{
		OwningPlayer->SetShowMouseCursor(false);
		OwningPlayer->SetInputMode(FInputModeGameOnly());
	}
}

void UFlipperTablistWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	if (LeaveSessionButton)
	{
		LeaveSessionButton->OnPressed.AddUniqueDynamic(this, &UFlipperTablistWidget::OnLeaveSessionButtonPressed);
	}
}

void UFlipperTablistWidget::NativeDestruct()
{
	if (LeaveSessionButton)
	{
		LeaveSessionButton->OnPressed.RemoveAll(this);
	}
	
	Super::NativeDestruct();
}

void UFlipperTablistWidget::RebuildTablist()
{
	PlayerRowsVerticalBox->ClearChildren();
	if (!RowWidgetClass) return;
	
	const UWorld* World = GetWorld();
	if (!World) return;
	
	const AGameStateBase* GameState = World->GetGameState();
	if (!GameState) return;
	
	// run through the game state's array of player states, casting and storing the valid flipper player states
	TArray<AFlipperPlayerState*> FlipperPlayerStates;
	for (auto PlayerState : GameState->PlayerArray)
	{
		AFlipperPlayerState* FlipperPlayerState = Cast<AFlipperPlayerState>(PlayerState);
		if (!FlipperPlayerState) continue;
		
		FlipperPlayerStates.Add(FlipperPlayerState);
	}

	// create a tablist row widget for each valid flipper player state in the game state player array
	for (const auto FlipperPlayerState : FlipperPlayerStates)
	{
		UFlipperTablistRowWidget* RowWidget = CreateWidget<UFlipperTablistRowWidget>(GetOwningPlayer(), RowWidgetClass);
		if (!RowWidget) continue;
		
		RowWidget->Setup(FlipperPlayerState);
		PlayerRowsVerticalBox->AddChildToVerticalBox(RowWidget);
	}
}

void UFlipperTablistWidget::OnLeaveSessionButtonPressed()
{
	const UFlipperGameInstance* GameInstance = GetGameInstance<UFlipperGameInstance>();
	UFlipperSessionSubsystem* SessionSubsystem = GameInstance ? GameInstance->GetSubsystem<UFlipperSessionSubsystem>() : nullptr;
	if (!SessionSubsystem) return;
	
	SessionSubsystem->LeaveSession();
}
