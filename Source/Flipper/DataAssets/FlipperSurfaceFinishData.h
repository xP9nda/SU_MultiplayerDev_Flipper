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
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UTexture2D> SurfaceTexture;
	
	// Whether the SurfaceTexture is applied as the target texture as well as the starting texture.
	//
	// For instance, something like wallpaper would want to adopt its end texture, but something like paint wouldn't change the underlying texture of the paintable
	// This only applies if this finish is used as a starting finish on a paintable
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool SurfaceTextureAppliedAsTarget = true;
};


/**
 * 
 */
UCLASS()
class FLIPPER_API UFlipperSurfaceFinishData : public UDataAsset
{
	GENERATED_BODY()
	
public:
	FFlipperSurfaceFinish* GetFinish(const FGameplayTag& Tag) { return Finishes.Find(Tag); } 
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Flipper|Surfaces", meta = (Categories = "Finish"))
	TMap<FGameplayTag, FFlipperSurfaceFinish> Finishes;
};
