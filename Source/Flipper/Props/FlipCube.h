// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
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
	float RotationSpeed = 45.0f;
};
