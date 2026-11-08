// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interactables/Instances/FlipperUnitsBaseInteractable.h"
#include "FlipperUnitSeller.generated.h"

struct FFlipperSurfaceFinish;

struct FFlipperSaleInformation
{
	UFlipperUsableUnitComponent* UnitsComponent = nullptr;
	const FFlipperSurfaceFinish* HeldSurfaceFinish = nullptr;
	uint8 UnitsToSell = 0;
	int32 TotalRefundValue = 0;
};

UCLASS()
class FLIPPER_API AFlipperUnitSeller : public AFlipperUnitsBaseInteractable
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AFlipperUnitSeller();

	virtual void Interact_Implementation(AActor* Interactor) override;
	virtual FFlipperInteractText GetInteractText(AActor* Interactor) override;

private:
	UPROPERTY(EditAnywhere, Category = "Flipper|Economy", meta = (ClampMin = "0", ClampMax = "1"))
	float SellPercentage = 0.75;
	
	// units that should be marked as "earned money", rather than a refund of already spent money
	UPROPERTY(EditAnywhere, Category = "Flipper|Economy", meta = (Categories = "Units"))
	FGameplayTagContainer UnitsToEarn;
	
	// units that are allowed to be sold at this unit seller
	UPROPERTY(EditAnywhere, Category = "Flipper|Economy", meta = (Categories = "Units"))
	FGameplayTagContainer AllowedSellableUnits;
	
	// whether the UI interaction text should show the sell percentage after the total value
	UPROPERTY(EditAnywhere, Category = "Flipper|UI", meta = (Categories = "Units"))
	bool bShouldShowSellPercentage;
	
	bool TryGetSaleInformation(AActor* Interactor, FFlipperSaleInformation& SaleInfo) const;
	
};
