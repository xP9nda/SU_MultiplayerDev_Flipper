// Fill out your copyright notice in the Description page of Project Settings.


#include "FlipperMenuSessionJoinableWidget.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Core/FlipperGameInstance.h"
#include "Subsystems/FlipperSessionSubsystem.h"

void UFlipperMenuSessionJoinableWidget::SetSessionResult(const FOnlineSessionSearchResult& InSessionResult)
{
	SessionResult = InSessionResult;
	const UFlipperGameInstance* GameInstance = GetGameInstance<UFlipperGameInstance>();
	
	SessionOwnerTextBlock->SetText(FText::Format(FText::FromString("{0}'s session"), FText::FromString(SessionResult.Session.OwningUserName)));
	SessionPingTextBlock->SetText(FText::Format(FText::FromString("{0}ms"), SessionResult.PingInMs));
	SessionPlayersTextBlock->SetText(FText::Format(FText::FromString("{0}/{1} players"),
		IsValid(GameInstance) ? FText::AsNumber(GameInstance->SessionMaxPublicConnections - SessionResult.Session.NumOpenPublicConnections) : FText::FromString("unknown"),
		IsValid(GameInstance) ? FText::AsNumber(GameInstance->SessionMaxPublicConnections) : FText::FromString("unknown")
	));
}

void UFlipperMenuSessionJoinableWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (JoinSessionButton)
	{
		JoinSessionButton->OnPressed.AddUniqueDynamic(this, &UFlipperMenuSessionJoinableWidget::OnJoinSessionButtonPressed);
	}
}

void UFlipperMenuSessionJoinableWidget::NativeDestruct()
{
	if (JoinSessionButton)
	{
		JoinSessionButton->OnPressed.RemoveAll(this);
	}
	
	Super::NativeDestruct();
}

void UFlipperMenuSessionJoinableWidget::OnJoinSessionButtonPressed()
{
	const UFlipperGameInstance* GameInstance = GetGameInstance<UFlipperGameInstance>();
	UFlipperSessionSubsystem* SessionSubsystem = GameInstance ? GameInstance->GetSubsystem<UFlipperSessionSubsystem>() : nullptr;
	if (!SessionSubsystem) return;
	
	JoinSessionButton->SetIsEnabled(false);
	ButtonTextBlock->SetText(FlipperTypes::GetStatusText(EFlipperSessionStatusText::JoiningSession));
	
	SessionSubsystem->OnFlipperJoinSessionComplete.AddUObject(this, &UFlipperMenuSessionJoinableWidget::OnJoinSessionComplete);
	SessionSubsystem->JoinGameSession(SessionResult);
	
}

void UFlipperMenuSessionJoinableWidget::OnJoinSessionComplete(EOnJoinSessionCompleteResult::Type Result)
{
	const UFlipperGameInstance* GameInstance = GetGameInstance<UFlipperGameInstance>();
	UFlipperSessionSubsystem* SessionSubsystem = GameInstance ? GameInstance->GetSubsystem<UFlipperSessionSubsystem>() : nullptr;
	if (!SessionSubsystem)
	{
		JoinSessionButton->SetIsEnabled(true);
		ButtonTextBlock->SetText(FlipperTypes::GetStatusText(EFlipperSessionStatusText::JoiningSessionIdle));
		return;
	}
	
	SessionSubsystem->OnFlipperJoinSessionComplete.RemoveAll(this);
	
	// if the result is not a successful join, allow retrying
	if (Result != EOnJoinSessionCompleteResult::Success)
	{
		JoinSessionButton->SetIsEnabled(true);
		ButtonTextBlock->SetText(FlipperTypes::GetStatusText(EFlipperSessionStatusText::JoiningSessionIdle));
		return;
	}
	
	// if the try travelling to current session fails, allow retrying
	if (!SessionSubsystem->TryTravelToCurrentSession())
	{
		JoinSessionButton->SetIsEnabled(true);
		ButtonTextBlock->SetText(FlipperTypes::GetStatusText(EFlipperSessionStatusText::JoiningSessionIdle));
		return;
	}
}
