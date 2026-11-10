// Fill out your copyright notice in the Description page of Project Settings.


#include "FlipperInteractCrosshairWidget.h"

#include "Components/FlipperInteractComponent.h"
#include "Components/Image.h"
#include "Interfaces/FlipperInteract.h"


void UFlipperInteractCrosshairWidget::NativePreConstruct()
{
	Super::NativePreConstruct();
	
	// default to the default brush state immediately
	if (CrosshairImage)
	{
		CrosshairImage->SetBrush(DefaultCrosshairBrush);
	}
}

void UFlipperInteractCrosshairWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	if (InteractProgressImage) InteractProgressImage->SetVisibility(ESlateVisibility::Collapsed);
	
	if (InteractProgressMaterial)
	{
		InteractProgressMaterialDynamic = UMaterialInstanceDynamic::Create(InteractProgressMaterial, this);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("InteractProgressMaterial is null on: %s"), *this->GetName());
	}
	
	if (InteractProgressMaterialDynamic.IsValid() && InteractProgressImage)
	{
		InteractProgressImage->SetBrushFromMaterial(InteractProgressMaterialDynamic.Get());
	}
	
	AController* OwningController = GetOwningPlayer();
	if (!OwningController) return;
	
	OwningController->OnPossessedPawnChanged.AddUniqueDynamic(this, &UFlipperInteractCrosshairWidget::HandlePossessedPawnChanged);
	HandlePossessedPawnChanged(nullptr, OwningController->GetPawn());
}

void UFlipperInteractCrosshairWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	if (!bIsAnimating) return;
	if (!InteractProgressMaterialDynamic.IsValid()) return;
	
	const double CurrentTime = GetWorld()->GetTimeSeconds();
	const double TimeRemaining = AnimationEndTime - CurrentTime;
	
	// start at 1 progress -> 0 when it ends so the ring ticks down from full to empty, implying a cooldown rather than a filling progress meter (or time for interact to happen)
	const float Progress = FMath::Clamp( (TimeRemaining / AnimationDuration), 0.0f, 1.0f);
	InteractProgressMaterialDynamic->SetScalarParameterValue(MaterialFillProgressParameterName, Progress);
	
	if (CurrentTime >= AnimationEndTime)
	{
		bIsAnimating = false;
		if (InteractProgressImage) InteractProgressImage->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UFlipperInteractCrosshairWidget::NativeDestruct()
{
	AController* OwningController = GetOwningPlayer();
	if (OwningController)
	{
		// unbind the pawn changed delegate
		OwningController->OnPossessedPawnChanged.RemoveDynamic(this, &UFlipperInteractCrosshairWidget::HandlePossessedPawnChanged);
	}
	
	// force no new component and therefore a clean-up on the old component bindings
	ObserveInteractComponent(nullptr);
	
	Super::NativeDestruct();
}

void UFlipperInteractCrosshairWidget::HandlePossessedPawnChanged(APawn* OldPawn, APawn* NewPawn)
{
	ObserveInteractComponent(NewPawn ? NewPawn->GetComponentByClass<UFlipperInteractComponent>() : nullptr);
}

void UFlipperInteractCrosshairWidget::HandleOnInteract()
{
	if (!IsValid(ObservedInteractComponent)) return;
	
	const double CurrentTime = GetWorld()->GetTimeSeconds();
	AnimationDuration = ObservedInteractComponent->InteractionCooldown;
	AnimationEndTime = CurrentTime + AnimationDuration;
	
	if (InteractProgressImage) InteractProgressImage->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	bIsAnimating = true;
}

void UFlipperInteractCrosshairWidget::HandleOnLookedAtActorChanged(AActor* LookedAtActor)
{
	if (!LookedAtActor || !LookedAtActor->Implements<UFlipperInteract>())
	{
		if (CrosshairImage)
		{
			CrosshairImage->SetBrush(DefaultCrosshairBrush);
		}
		return;
	}
	
	if (CrosshairImage)
	{
		CrosshairImage->SetBrush(HoverCrosshairBrush);
	}
}

void UFlipperInteractCrosshairWidget::ObserveInteractComponent(UFlipperInteractComponent* NewComponent)
{
	if (IsValid(ObservedInteractComponent))
	{
		ObservedInteractComponent->OnInteract.RemoveDynamic(this, &UFlipperInteractCrosshairWidget::HandleOnInteract);
		ObservedInteractComponent->OnLookedAtActorChanged.RemoveDynamic(this, &UFlipperInteractCrosshairWidget::HandleOnLookedAtActorChanged);
	}
	
	ObservedInteractComponent = NewComponent;
	if (IsValid(ObservedInteractComponent))
	{
		ObservedInteractComponent->OnInteract.AddUniqueDynamic(this, &UFlipperInteractCrosshairWidget::HandleOnInteract);
		ObservedInteractComponent->OnLookedAtActorChanged.AddUniqueDynamic(this, &UFlipperInteractCrosshairWidget::HandleOnLookedAtActorChanged);
	}
	
	// hide the widget if there's no interact component to observe
	if (!IsValid(ObservedInteractComponent))
	{
		if (InteractProgressImage) InteractProgressImage->SetVisibility(ESlateVisibility::Collapsed);
	}
}
