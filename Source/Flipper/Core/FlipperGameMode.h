// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "FlipperGameMode.generated.h"

class AFlipperJobSite;

UCLASS(Abstract)
class AFlipperGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	AFlipperGameMode();

	void RegisterJobSite(AFlipperJobSite* JobSite);
	
	UFUNCTION()
	void HandleJobSiteCompleted(const AFlipperJobSite* JobSite) const;
	
protected:
	virtual void BeginPlay() override;
	
private:
	UPROPERTY(EditDefaultsOnly)
	int32 StartingMoney = 100;
	
	TArray<TWeakObjectPtr<AFlipperJobSite>> JobSites;
};



