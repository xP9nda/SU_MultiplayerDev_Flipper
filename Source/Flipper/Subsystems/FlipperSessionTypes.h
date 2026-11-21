// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FlipperSessionTypes.generated.h"

UENUM(BlueprintType)
enum class EFlipperSessionStatusText : uint8
{
	FindingSession,
	FindingSessionIdle,
	
	JoiningSession,
	JoiningSessionIdle,
	
	StartingSession,
	StartingSessionIdle,
};

UENUM(BlueprintType)
enum class EFlipperSessionError : uint8
{
	ConnectionLost,
	ConnectionFailed,
	SessionFull,
	RejectedByHost,
	VersionMismatch,
	TravelFailed,
	UnknownNetworkError,
};

class FLIPPER_API FlipperTypes
{
public:
	static FText GetErrorText(EFlipperSessionError Error);
	static FText GetStatusText(EFlipperSessionStatusText Status);
};

