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
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (Categories = "Materials"))
	FGameplayTag MaterialTexture;
	
	// The paint that produces this finish when a paintable mesh targets it, e.g. Finish.Paint.Red for Finish.Brick.Red
	// leave empty on paint finishes, as they apply themselves
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (Categories = "Finish"))
	FGameplayTag AppliedByPaint;
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
	UTexture* GetMaterialTexture(const FGameplayTag& Tag) const { return MaterialTextures.Find(Tag) == nullptr ? nullptr : MaterialTextures.Find(Tag)->Get(); }
	
	// the finish a surface ends up with when painted with PaintFinish, given the finish that surface wants
	FGameplayTag ResolveAppliedFinish(const FGameplayTag& PaintFinish, const FGameplayTag& TargetFinish) const;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Flipper|Surfaces", meta = (Categories = "Finish"))
	TMap<FGameplayTag, FFlipperSurfaceFinish> Finishes;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Flipper|Surfaces", meta = (Categories = "Materials"))
	TMap<FGameplayTag, TObjectPtr<UTexture>> MaterialTextures;
};
