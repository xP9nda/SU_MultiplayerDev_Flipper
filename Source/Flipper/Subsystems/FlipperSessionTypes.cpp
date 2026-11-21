// Fill out your copyright notice in the Description page of Project Settings.


#include "FlipperSessionTypes.h"

FText FlipperTypes::GetErrorText(const EFlipperSessionError Error)
{
	switch (Error)
	{
		case EFlipperSessionError::ConnectionLost:
			return FText::FromString("Lost connection to the host");
		case EFlipperSessionError::ConnectionFailed:
			return FText::FromString("Could not connect to the host");
		case EFlipperSessionError::SessionFull:
			return FText::FromString("The server is full");
		case EFlipperSessionError::RejectedByHost:
			return FText::FromString("The host refused the connection");
		case EFlipperSessionError::VersionMismatch:
			return FText::FromString("You and the host are on different versions");
		case EFlipperSessionError::TravelFailed:
			return FText::FromString("Failed to load the map");
		case EFlipperSessionError::UnknownNetworkError:
			return FText::FromString("An unknown error occurred");
	}

	return FText::FromString("An unknown error occurred");
}

FText FlipperTypes::GetStatusText(const EFlipperSessionStatusText Status)
{
	switch (Status)
	{
		case EFlipperSessionStatusText::FindingSession:
			return FText::FromString("Finding games...");
		case EFlipperSessionStatusText::FindingSessionIdle:
			return FText::FromString("Find a game");
		case EFlipperSessionStatusText::JoiningSession:
			return FText::FromString("Joining...");
		case EFlipperSessionStatusText::JoiningSessionIdle:
			return FText::FromString("Join");
		case EFlipperSessionStatusText::StartingSession:
			return FText::FromString("Starting...");
		case EFlipperSessionStatusText::StartingSessionIdle:
			return FText::FromString("Start a game");
	}
	
	return FText::FromString("N/A SessionStatusText");
}
