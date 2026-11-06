// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interactables/Instances/FlipperUnitsBaseInteractable.h"
#include "FlipperUnitProvider.generated.h"

struct FFlipperSurfaceFinish;

struct FFlipperPurchaseInformation
{
	UFlipperUsableUnitComponent* UnitsComponent = nullptr;
	const FFlipperSurfaceFinish* ProvidedSurfaceFinish = nullptr;
	uint8 UnitsToPurchase = 0;
	int32 TotalCost = 0;
};

enum class EFlipperPurchaseResult : uint8
{
	Success,
	InvalidFinish, // the provided finish is unset or missing from the data asset
	WrongTool, // the interactor doesn't have the unit provider's tool equipped
	DifferentFinishHeld, // the interactor is holding units of a different finish than this provider
	Full // the interactor has no space left to fill
};

UCLASS()
class FLIPPER_API AFlipperUnitProvider : public AFlipperUnitsBaseInteractable
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AFlipperUnitProvider();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	void SetupMaterial();
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> FillStaticMeshComponent;
	
	UPROPERTY(EditAnywhere, Category = "Flipper|Config", meta = (Categories = "Finish"))
	FGameplayTag ProvidedFinish;
	
	UPROPERTY(EditAnywhere, Category = "Flipper|Mesh")
	TObjectPtr<UMaterialInterface> FillMaterial;
	
	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> FillMaterialDynamic;
	
	UPROPERTY(EditDefaultsOnly, Category = "Flipper|Mesh")
	FName MaterialProvidedColourParameterName = FName("ProvidedColour");
	
	UPROPERTY(EditDefaultsOnly, Category = "Flipper|Mesh")
	FName MaterialProvidedTextureParameterName = FName("ProvidedTexture");
	
	EFlipperPurchaseResult EvaluatePurchaseInformation(AActor* Interactor, FFlipperPurchaseInformation& PurchaseInfo) const;
	
public:
	virtual void Interact_Implementation(AActor* Interactor) override;
	virtual FFlipperInteractText GetInteractText(AActor* Interactor) override;
};
