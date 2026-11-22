// Fill out your copyright notice in the Description page of Project Settings.


#include "FlipperMenuHUDWidget.h"

#include "FlipperMenuSessionJoinableWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/WidgetSwitcher.h"
#include "Core/FlipperGameInstance.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Subsystems/FlipperSessionSubsystem.h"

void UFlipperMenuHUDWidget::SetMenuPageIndex(EFlipperMainMenuPageIndex NewIndex)
{
	if (!MenuWidgetSwitcher) return;
	
	// populate the results page whenever the found sessions page has been switched to
	// this ensures that whenever it is switched to, it is visually drawn with the last session search results, if any
	if (NewIndex == EFlipperMainMenuPageIndex::FoundSessions)
	{
		PopulateResultsPage();
	}
	
	MenuWidgetSwitcher->SetActiveWidgetIndex(NewIndex);
}

void UFlipperMenuHUDWidget::PopulateResultsPage()
{
	const UFlipperGameInstance* GameInstance = GetGameInstance<UFlipperGameInstance>();
	UFlipperSessionSubsystem* SessionSubsystem = GameInstance ? GameInstance->GetSubsystem<UFlipperSessionSubsystem>() : nullptr;
	if (!SessionSubsystem) return;
	
	const TArray<FOnlineSessionSearchResult>& SessionSearchResults = SessionSubsystem->GetLastSessionSearch()->SearchResults;
	if (SessionSearchResults.Num() == 0)
	{
		NumberOfSessionsTextBlock->SetText(FText::FromString("0 sessions found."));
		return;
	};
	
	NumberOfSessionsTextBlock->SetText(FText::Format(FText::FromString("{0} {0}|plural(one=session,other=sessions) found."),
		FText::AsNumber(SessionSearchResults.Num()))
	);
	
	SearchResultsVerticalBox->ClearChildren();
	for (FOnlineSessionSearchResult SessionResult : SessionSearchResults)
	{
		UFlipperMenuSessionJoinableWidget* SessionJoinableWidget = CreateWidget<UFlipperMenuSessionJoinableWidget>(GetOwningPlayer(), SearchResultWidgetClass);
		if (!SessionJoinableWidget) continue;
		
		SessionJoinableWidget->SetSessionResult(SessionResult);
		SearchResultsVerticalBox->AddChildToVerticalBox(SessionJoinableWidget);
	}
}

void UFlipperMenuHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	if (ReturnToCallToActionButton)
	{
		ReturnToCallToActionButton->OnPressed.AddUniqueDynamic(this, &UFlipperMenuHUDWidget::OnReturnToCallToActionButtonPressed);
	}
	
	if (CloseGameButton)
	{
		CloseGameButton->OnPressed.AddUniqueDynamic(this, &UFlipperMenuHUDWidget::OnCloseGameButtonPressed);
	}
}

void UFlipperMenuHUDWidget::NativeDestruct()
{
	if (ReturnToCallToActionButton)
	{
		ReturnToCallToActionButton->OnPressed.RemoveAll(this);
	}
	
	if (CloseGameButton)
	{
		CloseGameButton->OnPressed.RemoveAll(this);
	}
	
	Super::NativeDestruct();
}

void UFlipperMenuHUDWidget::OnReturnToCallToActionButtonPressed()
{
	SetMenuPageIndex(EFlipperMainMenuPageIndex::CallToAction);
}

void UFlipperMenuHUDWidget::OnCloseGameButtonPressed()
{
	UKismetSystemLibrary::QuitGame(this, GetOwningPlayer(), EQuitPreference::Quit, false);
}
