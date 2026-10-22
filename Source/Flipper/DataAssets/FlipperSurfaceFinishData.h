// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "FlipperSurfaceFinishData.generated.h"



USTRUCT(BlueprintType)
struct FLIPPER_API FFlipperSurfaceFinish
{
	GENERATED_BODY()
	
	// The name shown in UI - e.g. "Pale Yellow" "Yellow"
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText DisplayName;
	
	// The tint applied to the surface for the material
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FLinearColor SurfaceColour = FLinearColor::White;
	
	// The texture applied to the surface for the material
	// This is only applied to the start texture for a paintable mesh. Target texture remains as whatever the starting surface texture of the starting finish is
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UTexture2D> StartingSurfaceTexture;
	
	// The texture lerped towards as the target
	// This should be left null on most finishes, and is only to be used when the target texture may differ from the starting finish's texture
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UTexture2D> FinishedSurfaceTexture;
};


/**
 * 
 */
UCLASS()
class FLIPPER_API UFlipperSurfaceFinishData : public UDataAsset
{
	GENERATED_BODY()
	
public:
	// const so that the returned finish cannot be written into, read-only
	const FFlipperSurfaceFinish* GetFinish(const FGameplayTag& Tag) const { return Finishes.Find(Tag); } 
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Flipper|Surfaces", meta = (Categories = "Finish"))
	TMap<FGameplayTag, FFlipperSurfaceFinish> Finishes;
};
