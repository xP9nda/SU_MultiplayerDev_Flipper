// Fill out your copyright notice in the Description page of Project Settings.


#include "FlipperTablistRowWidget.h"

#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Core/FlipperPlayerState.h"
#include "GameFramework/GameModeBase.h"
#include "GameFramework/GameSession.h"

void UFlipperTablistRowWidget::Setup(AFlipperPlayerState* PlayerState)
{
	if (!PlayerState) return;
	
	ObservedPlayerState = PlayerState;
	
	// listen to contribution changes while this widget is active
	ObservedPlayerState->OnContributionChanged.AddUniqueDynamic(this, &UFlipperTablistRowWidget::OnContributionChanged);
	
	// if this row is for the local player, show the highlight border, if it's not, destroy it
	if (PlayerState == GetOwningPlayerState() && SelfHighlightBorder)
	{
		SelfHighlightBorder->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
	else
	{
		SelfHighlightBorder->RemoveFromParent();
	}

	const APlayerController* ObservedPlayerController = ObservedPlayerState->GetPlayerController();
	if (!ObservedPlayerController || ObservedPlayerController == GetOwningPlayer())
	{
		KickPlayerButton->SetVisibility(ESlateVisibility::Collapsed);
	}
	else
	{
		if (GetOwningPlayer()->HasAuthority())
		{
			KickPlayerButton->SetVisibility(ESlateVisibility::Visible);
			KickPlayerButton->OnPressed.AddUniqueDynamic(this, &UFlipperTablistRowWidget::OnKickPlayerButtonPressed);
		}
	}
	
	PostSetup();
	RefreshVisuals();
}

void UFlipperTablistRowWidget::NativeDestruct()
{
	AFlipperPlayerState* PlayerState = ObservedPlayerState.Get();
	if (PlayerState)
	{
		PlayerState->OnContributionChanged.RemoveDynamic(this, &UFlipperTablistRowWidget::OnContributionChanged);
	}
	
	Super::NativeDestruct();
}

void UFlipperTablistRowWidget::OnContributionChanged()
{
	RefreshVisuals();
}

void UFlipperTablistRowWidget::OnKickPlayerButtonPressed()
{
	// hide the kick button if the owning player isn't authority
	if (!GetOwningPlayer() || !GetOwningPlayer()->HasAuthority())
	{
		KickPlayerButton->SetVisibility(ESlateVisibility::Collapsed);
		return;
	}
	
	if (!ObservedPlayerState.IsValid()) return;

	const AGameModeBase* GameMode = GetWorld()->GetAuthGameMode();
	if (!GameMode || !GameMode->GameSession) return;
	
	APlayerController* TargetPlayerController = ObservedPlayerState->GetPlayerController();
	
	// don't allow kicking yourself
	if (!TargetPlayerController || TargetPlayerController == GetOwningPlayer()) return;
	
	GameMode->GameSession->KickPlayer(TargetPlayerController, FText::FromString("Kicked by host"));
}

void UFlipperTablistRowWidget::RefreshVisuals()
{
	AFlipperPlayerState* PlayerState = ObservedPlayerState.Get();
	if (!PlayerState) return;
	
	const FFlipperStepContribution& SurfaceContribution = PlayerState->GetSurfaceContribution();
	
	PlayerNameTextBlock->SetText(FText::FromString(PlayerState->GetPlayerName()));
	CorrectStepsTextBlock->SetText(FText::AsNumber(SurfaceContribution.CorrectSteps));
	IncorrectStepsTextBlock->SetText(FText::AsNumber(SurfaceContribution.IncorrectSteps));
	
	const FCulturePtr USCulture = FInternationalization::Get().GetCulture(TEXT("en-US")); // strip the "US" from "US$x.xx"
	const FText FormattedFundsEarned = FText::AsCurrencyBase(PlayerState->GetFundsEarned(), TEXT("USD"), USCulture);
	const FText FormattedFundsSpent = FText::AsCurrencyBase(PlayerState->GetFundsSpent(), TEXT("USD"), USCulture);
	FundsEarnedTextBlock->SetText(FormattedFundsEarned);
	FundsSpentTextBlock->SetText(FormattedFundsSpent);

	// run through all exposed pick up unit type text blocks and set the values of the ones that exist
	for (auto Element : PickupUnitTypeTextBlocks)
	{
		FFlipperPickedUpCount PickedUpCount = PlayerState->GetPickedUpCountForUnitType(Element.Key);
		if (!PickedUpCount.UnitType.IsValid())
		{
			// if the picked up count doesn't yet exist (i.e. player has not yet picked up any units of this type), default to showing 0
			Element.Value->SetText(FText::AsNumber(0));
			continue;
		};
		
		Element.Value->SetText(FText::AsNumber(PickedUpCount.Amount));
	}
}
