// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interactables/Instances/FlipperUnitsBaseInteractable.h"
#include "FlipperUnitProvider.generated.h"

UCLASS()
class FLIPPER_API AFlipperUnitProvider : public AFlipperUnitsBaseInteractable
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AFlipperUnitProvider();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	void SetupMaterial();
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> FillStaticMeshComponent;
	
	UPROPERTY(EditAnywhere, Category = "Flipper|Config", meta = (Categories = "Finish"))
	FGameplayTag ProvidedFinish;
	
	UPROPERTY(EditAnywhere, Category = "Flipper|Mesh")
	TObjectPtr<UMaterialInterface> FillMaterial;
	
	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> FillMaterialDynamic;
	
	UPROPERTY(EditDefaultsOnly, Category = "Flipper|Mesh")
	FName MaterialProvidedColourParameterName = FName("ProvidedColour");
	
	UPROPERTY(EditDefaultsOnly, Category = "Flipper|Mesh")
	FName MaterialProvidedTextureParameterName = FName("ProvidedTexture");
	
public:
	virtual void Interact_Implementation(AActor* Interactor) override;
};
