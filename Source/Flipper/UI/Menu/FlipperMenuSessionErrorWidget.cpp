// Fill out your copyright notice in the Description page of Project Settings.


#include "FlipperMenuSessionErrorWidget.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Subsystems/FlipperSessionSubsystem.h"

void UFlipperMenuSessionErrorWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	if (ConfirmButton)
	{
		ConfirmButton->OnPressed.AddUniqueDynamic(this, &UFlipperMenuSessionErrorWidget::OnConfirmButtonPressed);
	}
	
	// on construct, which will run any time the menu opens if this is placed on the menu hud widget
	if (ErrorTextBlock)
	{
		// get the subsystem's stored error message and set the text and visibility to it
		UFlipperSessionSubsystem* SessionSubsystem = GetGameInstance() ? GetGameInstance()->GetSubsystem<UFlipperSessionSubsystem>() : nullptr;
		const FText ErrorMessage = SessionSubsystem ? SessionSubsystem->UseAndClearPendingError() : FText::GetEmpty();
		
		ErrorTextBlock->SetText(ErrorMessage);
		SetVisibility(ErrorMessage.IsEmpty() ? ESlateVisibility::Collapsed : ESlateVisibility::Visible);
	}
}

void UFlipperMenuSessionErrorWidget::NativeDestruct()
{
	if (ConfirmButton)
	{
		ConfirmButton->OnPressed.RemoveAll(this);
	}
	
	Super::NativeDestruct();
}

void UFlipperMenuSessionErrorWidget::OnConfirmButtonPressed()
{
	// hide the error message widget when the confirm button is pressed
	SetVisibility(ESlateVisibility::Collapsed);
}
