// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interactables/Instances/FlipperUnitsBaseInteractable.h"
#include "FlipperUnitSeller.generated.h"

UCLASS()
class FLIPPER_API AFlipperUnitSeller : public AFlipperUnitsBaseInteractable
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AFlipperUnitSeller();
	
	virtual void Interact_Implementation(AActor* Interactor) override;

private:
	UPROPERTY(EditAnywhere, Category = "Flipper|Economy", meta = (ClampMin = "0", ClampMax = "1"))
	float SellPercentage = 0.75;
	
};
