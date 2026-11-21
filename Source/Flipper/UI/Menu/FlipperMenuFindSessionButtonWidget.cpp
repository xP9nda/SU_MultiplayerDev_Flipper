// Fill out your copyright notice in the Description page of Project Settings.


#include "FlipperMenuFindSessionButtonWidget.h"

#include "FlipperMenuHUDWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Core/FlipperGameInstance.h"
#include "Player/Menu/FlipperMenuHUD.h"
#include "Subsystems/FlipperSessionSubsystem.h"

void UFlipperMenuFindSessionButtonWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	if (FindSessionButton)
	{
		FindSessionButton->OnPressed.AddUniqueDynamic(this, &UFlipperMenuFindSessionButtonWidget::OnFindSessionButtonPressed);
	}
}

void UFlipperMenuFindSessionButtonWidget::NativeDestruct()
{
	if (FindSessionButton)
	{
		FindSessionButton->OnPressed.RemoveAll(this);
	}
	
	Super::NativeDestruct();
}

void UFlipperMenuFindSessionButtonWidget::OnFindSessionButtonPressed()
{
	const UFlipperGameInstance* GameInstance = GetGameInstance<UFlipperGameInstance>();
	UFlipperSessionSubsystem* SessionSubsystem = GameInstance ? GameInstance->GetSubsystem<UFlipperSessionSubsystem>() : nullptr;
	if (!SessionSubsystem) return;
	
	FindSessionButton->SetIsEnabled(false);
	ButtonTextBlock->SetText(FlipperTypes::GetStatusText(EFlipperSessionStatusText::FindingSession));
	
	SessionSubsystem->OnFlipperFindSessionsComplete.AddUObject(this, &UFlipperMenuFindSessionButtonWidget::OnFindSessionsComplete);
	SessionSubsystem->FindSessions(GameInstance->MaxSessionSearchResults, GameInstance->bSessionLanConnections);
}

void UFlipperMenuFindSessionButtonWidget::OnFindSessionsComplete(const TArray<FOnlineSessionSearchResult>& Results, bool bSuccessful)
{
	const UFlipperGameInstance* GameInstance = GetGameInstance<UFlipperGameInstance>();
	UFlipperSessionSubsystem* SessionSubsystem = GameInstance ? GameInstance->GetSubsystem<UFlipperSessionSubsystem>() : nullptr;
	if (!SessionSubsystem)
	{
		FindSessionButton->SetIsEnabled(true);
		ButtonTextBlock->SetText(FlipperTypes::GetStatusText(EFlipperSessionStatusText::FindingSessionIdle));
		return;
	}
	
	// unbind the delegate that was bound when the button was clicked
	SessionSubsystem->OnFlipperFindSessionsComplete.RemoveAll(this);
	
	// if not successful, then re-enable the button to allow retrying
	if (!bSuccessful)
	{
		FindSessionButton->SetIsEnabled(true);
		ButtonTextBlock->SetText(FlipperTypes::GetStatusText(EFlipperSessionStatusText::FindingSessionIdle));
		return;
	}
	
	// if successful, tell the HUD to show the session results instead of button content, then tell it to populate results page
	// 
	// this will move the player to the found sessions page even if no sessions were found, as populating that menu shows the number of sessions found,
	// showing 0 if none, so it provides feedback to the user
	const APlayerController* OwningPlayer = GetOwningPlayer();
	const AFlipperMenuHUD* OwningHUD = OwningPlayer ? OwningPlayer->GetHUD<AFlipperMenuHUD>() : nullptr;
	UFlipperMenuHUDWidget* MenuHUDWidget = OwningHUD ? OwningHUD->GetHUDWidget<UFlipperMenuHUDWidget>() : nullptr;
	
	if (MenuHUDWidget)
	{
		// set menu page index handles calling populate from previous search
		MenuHUDWidget->SetMenuPageIndex(EFlipperMainMenuPageIndex::FoundSessions);
	}
	
	// re-enable button to allow retrying and to re-open found sessions after finding them is complete
	FindSessionButton->SetIsEnabled(true);
	ButtonTextBlock->SetText(FlipperTypes::GetStatusText(EFlipperSessionStatusText::FindingSessionIdle));
}
