// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FlipperInteractComponent.generated.h"


class UCameraComponent;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class FLIPPER_API UFlipperInteractComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UFlipperInteractComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
	
	UFUNCTION()
	void AttemptInteract();
	
	UPROPERTY(EditInstanceOnly)
	float InteractionDistance = 500.0f;
	
private:
	UPROPERTY() // todo - document this
	TObjectPtr<UCameraComponent> OwnerCameraComponent;
	
	FHitResult PerformLinetrace();

	UFUNCTION(Server, Reliable, WithValidation, Category="Interaction") // - todo document
	void Server_AttemptInteraction();
};
