// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Net/UnrealNetwork.h"
#include "FlipCube.generated.h"

UCLASS()
class FLIPPER_API AFlipCube : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AFlipCube();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	
private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> MeshComponent;
	
	UPROPERTY(EditAnywhere)
	TObjectPtr<UStaticMesh> Mesh;
	
	UPROPERTY(EditAnywhere)
	TObjectPtr<UMaterialInterface> CubeMaterial;
	
	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> DynamicMaterial;
	
	UPROPERTY(EditAnywhere)
	TArray<FLinearColor> Colours;
	
	float TimeSinceLastColour = 0.0f;
	
	UPROPERTY(ReplicatedUsing = OnRep_MaterialColour)
	FLinearColor MaterialColour = FLinearColor::Black;
	
	UFUNCTION()
	void OnRep_MaterialColour();
	
	UPROPERTY(EditAnywhere)
	float TimeBetweenColourChanges = 5.0f;
	
	
	UPROPERTY(EditAnywhere)
	float RotationSpeed = 45.0f;

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
