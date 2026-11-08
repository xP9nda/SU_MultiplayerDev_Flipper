// Fill out your copyright notice in the Description page of Project Settings.


#include "FlipperInteractCooldownWidget.h"

#include "Components/FlipperInteractComponent.h"
#include "Components/Image.h"

void UFlipperInteractCooldownWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	
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
	
	OwningController->OnPossessedPawnChanged.AddUniqueDynamic(this, &UFlipperInteractCooldownWidget::HandlePossessedPawnChanged);
	HandlePossessedPawnChanged(nullptr, OwningController->GetPawn());
}

void UFlipperInteractCooldownWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	if (!bIsAnimating) return;
	if (!InteractProgressMaterialDynamic.IsValid()) return;
	
	const double CurrentTime = GetWorld()->GetTimeSeconds();
	const double TimeRemaining = AnimationEndTime - CurrentTime;
	
	// start at 0 progress -> 1 when it ends
	const float Progress = FMath::Clamp(1.0 - (TimeRemaining / AnimationDuration), 0.0f, 1.0f);
	InteractProgressMaterialDynamic->SetScalarParameterValue(MaterialFillProgressParameterName, Progress);
	
	if (CurrentTime >= AnimationEndTime)
	{
		bIsAnimating = false;
		SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UFlipperInteractCooldownWidget::NativeDestruct()
{
	AController* OwningController = GetOwningPlayer();
	if (OwningController)
	{
		// unbind the pawn changed delegate
		OwningController->OnPossessedPawnChanged.RemoveDynamic(this, &UFlipperInteractCooldownWidget::HandlePossessedPawnChanged);
	}
	
	// force no new component and therefore a clean-up on the old component bindings
	ObserveInteractComponent(nullptr);
	
	Super::NativeDestruct();
}

void UFlipperInteractCooldownWidget::HandlePossessedPawnChanged(APawn* OldPawn, APawn* NewPawn)
{
	ObserveInteractComponent(NewPawn ? NewPawn->GetComponentByClass<UFlipperInteractComponent>() : nullptr);
}

void UFlipperInteractCooldownWidget::HandleOnInteract()
{
	const double CurrentTime = GetWorld()->GetTimeSeconds();
	AnimationDuration = ObservedInteractComponent->InteractionCooldown;
	AnimationEndTime = CurrentTime + AnimationDuration;
	
	SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	bIsAnimating = true;
}

void UFlipperInteractCooldownWidget::ObserveInteractComponent(UFlipperInteractComponent* NewComponent)
{
	if (IsValid(ObservedInteractComponent))
	{
		ObservedInteractComponent->OnInteract.RemoveDynamic(this, &UFlipperInteractCooldownWidget::HandleOnInteract);
	}
	
	ObservedInteractComponent = NewComponent;
	if (IsValid(ObservedInteractComponent))
	{
		ObservedInteractComponent->OnInteract.AddUniqueDynamic(this, &UFlipperInteractCooldownWidget::HandleOnInteract);
	}
	
	// hide the widget if there's no interact component to observe
	if (!IsValid(ObservedInteractComponent))
	{
		SetVisibility(ESlateVisibility::Collapsed);
	}
}
