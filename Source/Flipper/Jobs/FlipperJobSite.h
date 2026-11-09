// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FlipperJobSite.generated.h"

class AFlipperUnitPickupMesh;
class AFlipperPaintableMesh;

UCLASS()
class FLIPPER_API AFlipperJobSite : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AFlipperJobSite();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	
private:
	UPROPERTY(EditInstanceOnly, Category = "Flipper|Job")
	TArray<TObjectPtr<AFlipperPaintableMesh>> PaintableMeshes;
	
	UPROPERTY(EditInstanceOnly, Category = "Flipper|Job")
	TArray<TObjectPtr<AFlipperUnitPickupMesh>> PickupMeshes;
	
	UPROPERTY(EditAnywhere, Category = "Flipper|Job", meta = (ClampMin = "0"))
	int32 CompletionPayout = 100;
};
