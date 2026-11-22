// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameSession.h"
#include "FlipperGameSession.generated.h"

UCLASS()
class FLIPPER_API AFlipperGameSession : public AGameSession
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AFlipperGameSession();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
};
