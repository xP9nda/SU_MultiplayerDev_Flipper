// Fill out your copyright notice in the Description page of Project Settings.


#include "FlipperTablistWidget.h"

#include "FlipperTablistRowWidget.h"
#include "Components/VerticalBox.h"
#include "Core/FlipperPlayerState.h"
#include "GameFramework/GameStateBase.h"

void UFlipperTablistWidget::ShowTablist()
{
	RebuildTablist();
	SetVisibility(ESlateVisibility::HitTestInvisible);
}

void UFlipperTablistWidget::HideTablist()
{
	SetVisibility(ESlateVisibility::Collapsed);
	PlayerRowsVerticalBox->ClearChildren();
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
