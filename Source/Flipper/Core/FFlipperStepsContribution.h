#pragma once

#include "CoreMinimal.h"
#include "FFlipperStepsContribution.generated.h"

UENUM()
enum EFlipperContributionType : uint8
{
	Correct,
	Incorrect
};

USTRUCT()
struct FFlipperStepsContribution
{
	GENERATED_BODY()
	
	UPROPERTY()
	int32 CorrectSteps = 0;
	
	UPROPERTY()
	int32 IncorrectSteps = 0;
};
