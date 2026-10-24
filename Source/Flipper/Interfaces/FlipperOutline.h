// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "FlipperOutline.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UFlipperOutline : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class FLIPPER_API IFlipperOutline
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	
	virtual void ShowOutline() = 0;
	virtual void HideOutline() = 0;
	
};
