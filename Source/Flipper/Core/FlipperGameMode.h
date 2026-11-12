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
	
	UFUNCTION()
	void HandleJobSiteCompleted(AFlipperJobSite* JobSite);
	
protected:
	virtual void BeginPlay() override;

public:
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void Logout(AController* Exiting) override;

private:
	UPROPERTY(EditDefaultsOnly)
	int32 StartingMoney = 100;
};



