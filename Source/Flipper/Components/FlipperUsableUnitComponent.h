// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "FlipperUsableUnitComponent.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnUsableUnitsChanged);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCurrentFinishChanged);

/// Usable Unit Component
/// Refers to any "usable units" that a player may have
/// for instance, a usable unit quantity of paint, wallpaper, etc
/// Each instance of usable units (paint, wallpaper, etc) should have its own copy of this usable unit component
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class FLIPPER_API UFlipperUsableUnitComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UFlipperUsableUnitComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
	
private:
	UPROPERTY(EditDefaultsOnly, ReplicatedUsing = OnRep_UsableUnits, Category = "Units")
	uint8 UsableUnits = 0;
	
	UPROPERTY(EditAnywhere, Category = "Units")
	uint8 MaxUsableUnits = 20;
	
	UPROPERTY(ReplicatedUsing = OnRep_CurrentFinish)
	FGameplayTag CurrentFinish;
	
	UFUNCTION()
	void OnRep_UsableUnits() const;
	
	UFUNCTION()
	void OnRep_CurrentFinish() const;
	
	
	
	UPROPERTY(EditDefaultsOnly, Category = "Units", meta = (Categories = "Units"))
	FGameplayTag UnitType;
	
public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	// public so that interactables can spend units
	bool TryConsumeUnit(uint8 Quantity);
	
	// public so that interactables can add units
	bool TryAddUnit(uint8 Quantity);
	
	FOnUsableUnitsChanged OnUsableUnitsChanged;
	FOnCurrentFinishChanged OnCurrentFinishChanged;
	
	// getters
	// getter for max usable units so that it can be used in HUD to display max quantity of units that can be held
	uint8 GetMaxUsableUnits() const {return MaxUsableUnits;}
	
	// getter for currently available units so that it can be used in HUD and on interactables to calculate filling to max  
	uint8 GetUsableUnits() const {return UsableUnits;}
	
	// getter for current finish so that it can be retrieved by paintables to apply desired finish onto it
	FGameplayTag GetCurrentFinish() const {return CurrentFinish;}
	
	// setters
	void SetCurrentFinish(const FGameplayTag NewCurrentFinish);
	
	// static helper to try and find units component of type, given an actor and a gameplay tag (that refers to `UnitType` property)
	// used to retrieve separate units across multiple instances of UFlipperUsableUnitComponent on an actor
	static UFlipperUsableUnitComponent* FindUnitComponentOfType(const AActor* Actor, const FGameplayTag& InTag);
};
