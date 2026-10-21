// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/FlipperInteract.h"
#include "FlipperBaseInteractable.generated.h"

UCLASS(Abstract)
class FLIPPER_API AFlipperBaseInteractable : public AActor, public IFlipperInteract //  - todo document
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AFlipperBaseInteractable();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	UFUNCTION(Blueprintable)
	virtual void Interact_Implementation(AActor* Interactor) override;
};
