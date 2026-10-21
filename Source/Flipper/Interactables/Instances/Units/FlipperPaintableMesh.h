// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interactables/Instances/FlipperUnitsBaseInteractable.h"
#include "FlipperPaintableMesh.generated.h"

UCLASS()
class FLIPPER_API AFlipperPaintableMesh : public AFlipperUnitsBaseInteractable
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
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> StaticMeshComponent;
	
	UPROPERTY(EditAnywhere, Category = "Flipper|Mesh")
	TObjectPtr<UMaterialInterface> DesiredMaterial;
	
	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> DesiredMaterialDynamic;
	
	UPROPERTY(EditDefaultsOnly, Category = "Flipper|Mesh")
	FName MaterialProgressParameterName = FName("PaintProgress");
	
	UPROPERTY(EditDefaultsOnly, Category = "Flipper|Mesh")
	FName MaterialStartTextureParameterName = FName("StartTexture");
	
	UPROPERTY(EditDefaultsOnly, Category = "Flipper|Mesh")
	FName MaterialTargetTextureParameterName = FName("TargetTexture");
	
	UPROPERTY(EditDefaultsOnly, Category = "Flipper|Mesh")
	FName MaterialStartColourParameterName = FName("StartColour");
	
	UPROPERTY(EditDefaultsOnly, Category = "Flipper|Mesh")
	FName MaterialTargetColourParameterName = FName("TargetColour");
	
	UPROPERTY(EditAnywhere, Category = "Flipper|Config")
	uint8 MaxPaintableSteps = 20;
	
	UPROPERTY(ReplicatedUsing = OnRep_PaintState)
	uint8 CurrentPaintableStep = 0;
	
	// The starting finish of the paintable mesh, i.e. whatever the default mesh appearance should be
	UPROPERTY(EditAnywhere, Category = "Flipper|Config", meta = (Categories = "Finish"))
	FGameplayTag StartFinish;
	
	// The target finish of the paintable mesh, i.e. what this should end up as to be painted correctly
	UPROPERTY(EditAnywhere, Category = "Flipper|Config", meta = (Categories = "Finish"))
	FGameplayTag TargetFinish;
	
	// The current finish being applied to the paintable mesh, i.e. whatever a user who paints finish is
	UPROPERTY(ReplicatedUsing = OnRep_PaintState)
	FGameplayTag CurrentFinish;
	
	UFUNCTION()
	void OnRep_PaintState();
	
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void Interact_Implementation(AActor* Interactor) override;
};
