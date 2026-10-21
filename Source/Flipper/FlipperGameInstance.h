// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DataAssets/FlipperSurfaceFinishData.h"
#include "Engine/GameInstance.h"
#include "FlipperGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class FLIPPER_API UFlipperGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Flipper|Surfaces")
	TObjectPtr<UFlipperSurfaceFinishData> SurfaceData;
};
