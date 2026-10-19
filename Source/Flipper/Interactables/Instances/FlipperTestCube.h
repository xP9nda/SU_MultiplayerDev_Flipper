// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interactables/FlipperBaseInteractable.h"
#include "FlipperTestCube.generated.h"

UCLASS()
class FLIPPER_API AFlipperTestCube : public AFlipperBaseInteractable
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AFlipperTestCube();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UStaticMeshComponent> StaticMeshComponent;
	
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UMaterialInterface> InteractedMaterial;
	
	UPROPERTY(ReplicatedUsing = OnRep_IsInteracted) // todo - document 
	bool bIsInteracted;
	
	UFUNCTION()
	void OnRep_IsInteracted();
	
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void Interact_Implementation() override;
};
