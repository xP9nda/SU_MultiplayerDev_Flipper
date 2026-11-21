#pragma once

#include "CoreMinimal.h"
#include "FlipperStepTypes.generated.h"

UENUM()
enum EFlipperContributionType : uint8
{
	Correct,
	Incorrect
};

USTRUCT()
struct FFlipperStepContribution
{
	GENERATED_BODY()
	
	UPROPERTY()
	int32 CorrectSteps = 0;
	
	UPROPERTY()
	int32 IncorrectSteps = 0;
};
