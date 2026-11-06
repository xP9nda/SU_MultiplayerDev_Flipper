// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "FlipperInteract.generated.h"


USTRUCT()
struct FFlipperInteractText
{
	GENERATED_BODY()
	
	FText MainText;
	FText DescriptionText;
};


// This class does not need to be modified.
UINTERFACE()
class UFlipperInteract : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class FLIPPER_API IFlipperInteract
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void Interact(AActor* Interactor);

	virtual FFlipperInteractText GetInteractText(AActor* Interactor) = 0;
};
