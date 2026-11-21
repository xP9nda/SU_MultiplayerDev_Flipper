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
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Flipper|Tools", meta = (Categories = "Units"))
	TMap<FGameplayTag, FText> ToolNames;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Flipper|Settings")
	TSoftObjectPtr<UWorld> MenuLevel;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Flipper|Settings")
	TSoftObjectPtr<UWorld> GameLevel;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Flipper|Settings")
	int32 SessionMaxPublicConnections = 4;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Flipper|Settings")
	int32 MaxSessionSearchResults = 50;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Flipper|Settings")
	bool bSessionLanConnections = true;
};
