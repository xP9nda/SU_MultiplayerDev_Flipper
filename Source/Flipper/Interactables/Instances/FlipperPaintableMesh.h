// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interactables/FlipperBaseInteractable.h"
#include "FlipperPaintableMesh.generated.h"

UCLASS()
class FLIPPER_API AFlipperPaintableMesh : public AFlipperBaseInteractable
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AFlipperPaintableMesh();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	void SetupMaterial();
	
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UStaticMeshComponent> StaticMeshComponent;
	
	UPROPERTY(EditAnywhere, Category = "Flipper|Mesh")
	TObjectPtr<UMaterialInterface> DesiredMaterial;
	
	UPROPERTY(EditDefaultsOnly, Category = "Flipper|Mesh")
	FName MaterialProgressParameterName = FName("PaintProgress");
	
	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> DesiredMaterialDynamic;
	
	UPROPERTY(EditAnywhere, Category = "Flipper|Config")
	uint8 MaxPaintableSteps = 20;
	
	UPROPERTY(ReplicatedUsing = OnRep_CurrentPaintableStep)
	uint8 CurrentPaintableStep = 0;
	
	UFUNCTION()
	void OnRep_CurrentPaintableStep();
	
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void OnConstruction(const FTransform& Transform) override;
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void Interact_Implementation() override;
};
