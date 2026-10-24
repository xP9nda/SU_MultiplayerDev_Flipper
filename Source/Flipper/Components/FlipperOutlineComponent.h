// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Interfaces/FlipperOutline.h"
#include "FlipperOutlineComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class FLIPPER_API UFlipperOutlineComponent : public UActorComponent, public IFlipperOutline
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UFlipperOutlineComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:
	TArray<TObjectPtr<UStaticMeshComponent>> OwnerStaticMeshes;
	
public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
	
	virtual void ShowOutline() override;
	virtual void HideOutline() override;
};
