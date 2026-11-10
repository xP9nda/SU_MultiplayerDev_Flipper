// Fill out your copyright notice in the Description page of Project Settings.


#include "FlipperTeamFundsWidget.h"

#include "Components/TextBlock.h"
#include "Core/FlipperGameState.h"

void UFlipperTeamFundsWidget::NativeConstruct()
{
	Super::NativeConstruct();

	const UWorld* World = GetWorld();
	if (!World) return;
	
	AFlipperGameState* GameState = GetWorld()->GetGameState<AFlipperGameState>();
	if (!GameState) return;
	
	ObservedGameState = GameState;
	ObservedGameState->OnTeamFundsChanged.AddUniqueDynamic(this, &UFlipperTeamFundsWidget::HandleTeamFundsChanged);
	HandleTeamFundsChanged(ObservedGameState->GetTeamFunds()); // update immediately with the current team funds
}

void UFlipperTeamFundsWidget::NativeDestruct()
{
	if (IsValid(ObservedGameState.Get()))
	{
		ObservedGameState->OnTeamFundsChanged.RemoveDynamic(this, &UFlipperTeamFundsWidget::HandleTeamFundsChanged);
	}
	
	Super::NativeDestruct();
	
}

void UFlipperTeamFundsWidget::HandleTeamFundsChanged(int32 NewFunds)
{
	if (!IsValid(TeamFundsText)) return;

	const FCulturePtr USCulture = FInternationalization::Get().GetCulture(TEXT("en-US")); // strip the "US" from "US$x.xx"
	const FText FormattedFunds = FText::AsCurrencyBase(NewFunds, TEXT("USD"), USCulture);
	TeamFundsText->SetText(FormattedFunds);
}
