// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "FlipperToolComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnToolChanged, FGameplayTag, NewTool);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class FLIPPER_API UFlipperToolComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UFlipperToolComponent();
	
	void RequestSetTool(FGameplayTag NewTool);
	
	void CycleTool(int32 Direction); // +-1, wraps AvailableTools and sends RequestSetTool
	
	FGameplayTag GetCurrentTool() const { return CurrentTool; }
	const TArray<FGameplayTag>& GetAvailableTools() const { return AvailableTools; }
	
	FOnToolChanged OnToolChanged;
	
private:
	UPROPERTY(EditDefaultsOnly, Category = "Flipper|Tools", meta = (Categories = "Units"))
	TArray<FGameplayTag> AvailableTools;
	
	UPROPERTY(ReplicatedUsing = OnRep_CurrentTool)
	FGameplayTag CurrentTool;
	
	UFUNCTION(Server, Reliable)
	void Server_SetCurrentTool(FGameplayTag NewTool);
	
	UFUNCTION(Client, Reliable)
	void Client_RejectToolChange(FGameplayTag RejectedTool, FGameplayTag AuthoritativeTool);
	
	UFUNCTION()
	void OnRep_CurrentTool();
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
