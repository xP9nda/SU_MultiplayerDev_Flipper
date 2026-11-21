// Fill out your copyright notice in the Description page of Project Settings.


#include "FlipperMenuStartSessionButtonWidget.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Core/FlipperGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Subsystems/FlipperSessionSubsystem.h"

void UFlipperMenuStartSessionButtonWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	if (StartSessionButton)
	{
		StartSessionButton->OnPressed.AddUniqueDynamic(this, &UFlipperMenuStartSessionButtonWidget::OnStartSessionButtonPressed);
	}
}

void UFlipperMenuStartSessionButtonWidget::NativeDestruct()
{
	if (StartSessionButton)
	{
		StartSessionButton->OnPressed.RemoveAll(this);
	}
	
	const UFlipperGameInstance* GameInstance = GetGameInstance<UFlipperGameInstance>();
	UFlipperSessionSubsystem* SessionSubsystem = GameInstance ? GameInstance->GetSubsystem<UFlipperSessionSubsystem>() : nullptr;
	if (SessionSubsystem)
	{
		SessionSubsystem->OnFlipperCreateSessionComplete.RemoveAll(this);
		SessionSubsystem->OnFlipperStartSessionComplete.RemoveAll(this);
	}
	
	Super::NativeDestruct();
}

void UFlipperMenuStartSessionButtonWidget::OnStartSessionButtonPressed()
{
	const UFlipperGameInstance* GameInstance = GetGameInstance<UFlipperGameInstance>();
	UFlipperSessionSubsystem* SessionSubsystem = GameInstance ? GameInstance->GetSubsystem<UFlipperSessionSubsystem>() : nullptr;
	if (!SessionSubsystem) return;
	
	StartSessionButton->SetIsEnabled(false);
	ButtonTextBlock->SetText(FlipperTypes::GetStatusText(EFlipperSessionStatusText::StartingSession));
	
	SessionSubsystem->OnFlipperCreateSessionComplete.AddUniqueDynamic(this, &UFlipperMenuStartSessionButtonWidget::OnCreateSessionComplete);
	SessionSubsystem->CreateSession(GameInstance->SessionMaxPublicConnections, GameInstance->bSessionLanConnections);
}

void UFlipperMenuStartSessionButtonWidget::OnCreateSessionComplete(bool bSuccessful)
{
	const UFlipperGameInstance* GameInstance = GetGameInstance<UFlipperGameInstance>();
	UFlipperSessionSubsystem* SessionSubsystem = GameInstance ? GameInstance->GetSubsystem<UFlipperSessionSubsystem>() : nullptr;
	if (!SessionSubsystem)
	{
		StartSessionButton->SetIsEnabled(true);
		ButtonTextBlock->SetText(FlipperTypes::GetStatusText(EFlipperSessionStatusText::StartingSessionIdle));
		return;
	}
	
	// unbind the start delegate that was bound when the button was clicked
	SessionSubsystem->OnFlipperCreateSessionComplete.RemoveAll(this);
	
	// if not successful, re-enable the button to allow retrying
	if (!bSuccessful)
	{
		StartSessionButton->SetIsEnabled(true);
		ButtonTextBlock->SetText(FlipperTypes::GetStatusText(EFlipperSessionStatusText::StartingSessionIdle));
		return;
	}
	
	// if it was successful, call to start the session
	SessionSubsystem->OnFlipperStartSessionComplete.AddUniqueDynamic(this, &UFlipperMenuStartSessionButtonWidget::OnStartSessionComplete);
	SessionSubsystem->StartSession();
}

// ran when the session has been completed, and then started
void UFlipperMenuStartSessionButtonWidget::OnStartSessionComplete(bool bSuccessful)
{
	const UFlipperGameInstance* GameInstance = GetGameInstance<UFlipperGameInstance>();
	UFlipperSessionSubsystem* SessionSubsystem = GameInstance ? GameInstance->GetSubsystem<UFlipperSessionSubsystem>() : nullptr;
	if (!bSuccessful || !IsValid(GameInstance))
	{
		StartSessionButton->SetIsEnabled(true);
		ButtonTextBlock->SetText(FlipperTypes::GetStatusText(EFlipperSessionStatusText::StartingSessionIdle));
		return;
	}
	
	SessionSubsystem->OnFlipperStartSessionComplete.RemoveAll(this);
	
	UGameplayStatics::OpenLevelBySoftObjectPtr(GetWorld(), GameInstance->GameLevel, true, FString("listen"));
	// UGameplayStatics::OpenLevel(GetWorld(), *GameInstance->GameLevel.GetAssetName(), true, FString("listen"));
}
