// Fill out your copyright notice in the Description page of Project Settings.


#include "FlipperSessionSubsystem.h"

#include "Flipper.h"
#include "OnlineSessionSettings.h"
#include "OnlineSubsystemUtils.h"
#include "Core/FlipperGameInstance.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Online/OnlineSessionNames.h"

DEFINE_LOG_CATEGORY_STATIC(LogFlipperSessionSubsystem, Log, All);

#define FLIPPER_LOG(Verbosity, Format, ...) \
	UE_LOG(LogFlipper, Verbosity, TEXT("[%hs:%d] ") TEXT(Format), __FUNCTION__, __LINE__, ##__VA_ARGS__)

void UFlipperSessionSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	const IOnlineSessionPtr Sessions = GetSessionInterface();
	if (Sessions.IsValid())
	{
		OnCreateSessionComplete = FOnCreateSessionCompleteDelegate::CreateUObject(this, &UFlipperSessionSubsystem::OnCreateSessionCompleted);
		OnStartSessionComplete = FOnStartSessionCompleteDelegate::CreateUObject(this, &UFlipperSessionSubsystem::OnStartSessionCompleted);
		OnDestroySessionComplete = FOnDestroySessionCompleteDelegate::CreateUObject(this, &UFlipperSessionSubsystem::OnDestroySessionCompleted);
		OnFindSessionsComplete = FOnFindSessionsCompleteDelegate::CreateUObject(this, &UFlipperSessionSubsystem::OnFindSessionsCompleted);
		OnJoinSessionComplete = FOnJoinSessionCompleteDelegate::CreateUObject(this, &UFlipperSessionSubsystem::OnJoinSessionCompleted);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No online session interface found"));
	}
	
	// bind into network failure handle as a host quitting, timeouts, kicks, etc will be picked up here
	if (GEngine)
	{
		NetworkFailureHandle = GEngine->OnNetworkFailure().AddUObject(this, &UFlipperSessionSubsystem::HandleNetworkFailure);
		TravelFailureHandle = GEngine->OnTravelFailure().AddUObject(this, &UFlipperSessionSubsystem::HandleTravelFailure);
	}
	
	// bind to after each map load so that we can clear the leaving state
	PostLoadMapHandle = FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &UFlipperSessionSubsystem::HandlePostLoadMap);
}

void UFlipperSessionSubsystem::Deinitialize()
{
	const IOnlineSessionPtr Sessions = GetSessionInterface();
	if (Sessions.IsValid())
	{
		OnCreateSessionComplete.Unbind();
		OnStartSessionComplete.Unbind();
		OnDestroySessionComplete.Unbind();
		OnFindSessionsComplete.Unbind();
		OnJoinSessionComplete.Unbind();
	}
	
	if (GEngine)
	{
		GEngine->OnNetworkFailure().Remove(NetworkFailureHandle);
		GEngine->OnTravelFailure().Remove(TravelFailureHandle);
	}
	
	FCoreUObjectDelegates::PostLoadMapWithWorld.Remove(PostLoadMapHandle);

	Super::Deinitialize();
}

// -------- Create Session
void UFlipperSessionSubsystem::CreateSession(int32 NumPublicConnections, bool bIsLanMatch)
{
	FLIPPER_LOG(Log, "Creating session with, Max Players: %d", NumPublicConnections);
	
	const IOnlineSessionPtr Sessions = GetSessionInterface();
	if (!Sessions.IsValid())
	{
		FLIPPER_LOG(Error, "Sessions Invalid");
		OnFlipperCreateSessionComplete.Broadcast(false);
		return;
	};
	
	LastSessionSettings = MakeShareable(new FOnlineSessionSettings());
	
	LastSessionSettings->NumPrivateConnections = 0;
	LastSessionSettings->NumPublicConnections = NumPublicConnections;
	LastSessionSettings->bAllowInvites = true;
	LastSessionSettings->bAllowJoinInProgress = true;
	LastSessionSettings->bAllowJoinViaPresence = true;
	LastSessionSettings->bAllowJoinViaPresenceFriendsOnly = true;
	LastSessionSettings->bIsDedicated = false;
	LastSessionSettings->bUsesPresence = true;
	LastSessionSettings->bIsLANMatch = bIsLanMatch;
	LastSessionSettings->bShouldAdvertise = true;
	
	const UFlipperGameInstance* GameInstance = Cast<UFlipperGameInstance>(GetGameInstance());
	if (!IsValid(GameInstance))
	{
		FLIPPER_LOG(Error, "Game instance is null");
		OnFlipperCreateSessionComplete.Broadcast(false);
		return;
	}
	FLIPPER_LOG(Log, "GameInstance class: %s", *GameInstance->GetClass()->GetName());
	
	if (GameInstance->GameLevel.IsNull()) // changed from an IsValid check on GameLevel as it's a soft object ptr (IsValid returns if its loaded)
	{
		FLIPPER_LOG(Error, "Game instance's GameLevel is null");
		OnFlipperCreateSessionComplete.Broadcast(false);
		return;
	}  
	
	FLIPPER_LOG(Log, "Game level asset path: %s", *GameInstance->GameLevel.ToSoftObjectPath().ToString());
	FLIPPER_LOG(Log, "Game level asset name: %s", *GameInstance->GameLevel.GetAssetName());
	
	LastSessionSettings->Set(SETTING_MAPNAME, GameInstance->GameLevel.GetAssetName(), EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	CreateSessionCompleteHandle = Sessions->AddOnCreateSessionCompleteDelegate_Handle(OnCreateSessionComplete);
	
	const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	if (!Sessions->CreateSession(*LocalPlayer->GetPreferredUniqueNetId(), NAME_GameSession, *LastSessionSettings))
	{
		Sessions->ClearOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteHandle);
		OnFlipperCreateSessionComplete.Broadcast(false);
	}
}

void UFlipperSessionSubsystem::OnCreateSessionCompleted(FName SessionName, bool bSuccessful)
{
	FLIPPER_LOG(Log, "Create session completed, successful: %s", bSuccessful ? TEXT("true") : TEXT("false"));
	
	const IOnlineSessionPtr Sessions = GetSessionInterface();
	if (Sessions.IsValid())
	{
		Sessions->ClearOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteHandle);
	}
	
	OnFlipperCreateSessionComplete.Broadcast(bSuccessful);
}

// -------- Start Session
void UFlipperSessionSubsystem::StartSession()
{
	FLIPPER_LOG(Log, "Starting session");
	
	const IOnlineSessionPtr Sessions = GetSessionInterface();
	if (!Sessions.IsValid())
	{
		FLIPPER_LOG(Warning, "sessions invalid");
		OnFlipperStartSessionComplete.Broadcast(false);
		return;
	}
	
	StartSessionCompleteHandle = Sessions->AddOnStartSessionCompleteDelegate_Handle(OnStartSessionComplete);
	
	if (!Sessions->StartSession(NAME_GameSession))
	{
		Sessions->ClearOnStartSessionCompleteDelegate_Handle(StartSessionCompleteHandle);
		OnFlipperStartSessionComplete.Broadcast(false);
	}
}

void UFlipperSessionSubsystem::OnStartSessionCompleted(FName SessionName, bool bSuccessful)
{
	FLIPPER_LOG(Log, "Start session finished, successful: %s", bSuccessful ? TEXT("true") : TEXT("false"));
	
	const IOnlineSessionPtr Sessions = GetSessionInterface();
	if (Sessions.IsValid())
	{
		Sessions->ClearOnStartSessionCompleteDelegate_Handle(StartSessionCompleteHandle);
	}
	
	OnFlipperStartSessionComplete.Broadcast(bSuccessful);
}

// -------- Destroy Session
void UFlipperSessionSubsystem::DestroySession()
{
	FLIPPER_LOG(Log, "Destroying session");
	
	const IOnlineSessionPtr Sessions = GetSessionInterface();
	if (!Sessions.IsValid())
	{
		FLIPPER_LOG(Warning, "sessions invalid");
		FinishDestroySession(false);
		return;
	}
	
	// return a successful destruction if the session is already deleted
	if (!Sessions->GetNamedSession(NAME_GameSession))
	{
		FinishDestroySession(true);
		return;
	}
	
	DestroySessionCompleteHandle = Sessions->AddOnDestroySessionCompleteDelegate_Handle(OnDestroySessionComplete);
	if (!Sessions->DestroySession(NAME_GameSession))
	{
		if (DestroySessionCompleteHandle.IsValid())
		{
			Sessions->ClearOnDestroySessionCompleteDelegate_Handle(DestroySessionCompleteHandle);
			FinishDestroySession(false);
		}
	}
}

void UFlipperSessionSubsystem::OnDestroySessionCompleted(FName SessionName, bool bSuccessful)
{
	FLIPPER_LOG(Log, "Destroy session finished, successful: %s", bSuccessful ? TEXT("true") : TEXT("false"));
	
	const IOnlineSessionPtr Sessions = GetSessionInterface();
	if (Sessions.IsValid())
	{
		Sessions->ClearOnDestroySessionCompleteDelegate_Handle(DestroySessionCompleteHandle);
	}
	
	FinishDestroySession(bSuccessful);
}

// -------- Find Session
void UFlipperSessionSubsystem::FindSessions(int32 MaxSearchResults, bool bIsLanQuery)
{
	FLIPPER_LOG(Log, "Finding sessions");
	
	const IOnlineSessionPtr Sessions = GetSessionInterface();
	if (!Sessions.IsValid())
	{
		FLIPPER_LOG(Warning, "sessions invalid");
		OnFlipperFindSessionsComplete.Broadcast(TArray<FOnlineSessionSearchResult>(), false);
		return;
	}
	
	FindSessionsCompleteHandle = Sessions->AddOnFindSessionsCompleteDelegate_Handle(OnFindSessionsComplete);
	
	LastSessionSearch = MakeShareable(new FOnlineSessionSearch());
	
	LastSessionSearch->MaxSearchResults = MaxSearchResults;
	LastSessionSearch->bIsLanQuery = bIsLanQuery;
	LastSessionSearch->QuerySettings.Set("SEARCH_PRESENCE", true, EOnlineComparisonOp::Equals);
	
	const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	if (!Sessions->FindSessions(*LocalPlayer->GetPreferredUniqueNetId(), LastSessionSearch.ToSharedRef()))
	{
		Sessions->ClearOnFindSessionsCompleteDelegate_Handle(FindSessionsCompleteHandle);
		OnFlipperFindSessionsComplete.Broadcast(TArray<FOnlineSessionSearchResult>(), false);
	}
}

void UFlipperSessionSubsystem::OnFindSessionsCompleted(bool bSuccessful)
{
	FLIPPER_LOG(Log, "Finding sessions completed, successful: %s", bSuccessful ? TEXT("true") : TEXT("false"));
	
	const IOnlineSessionPtr Sessions = GetSessionInterface();
	if (Sessions.IsValid())
	{
		Sessions->ClearOnFindSessionsCompleteDelegate_Handle(FindSessionsCompleteHandle);
	}
	
	FLIPPER_LOG(Log, "Found Results: %d", LastSessionSearch->SearchResults.Num());
	
	if (LastSessionSearch->SearchResults.Num() <= 0)
	{
		// broadcast a successful search here (if it was), so that the UI can take the user to the 
		// sessions page and show "0 sessions found", providing feedback to the user
		OnFlipperFindSessionsComplete.Broadcast(TArray<FOnlineSessionSearchResult>(), bSuccessful);
		return;
	} 
	
	OnFlipperFindSessionsComplete.Broadcast(LastSessionSearch->SearchResults, bSuccessful);
}

// -------- Join & Travel To Session
void UFlipperSessionSubsystem::JoinGameSession(const FOnlineSessionSearchResult& SessionResult)
{
	FLIPPER_LOG(Log, "Joining Game Session");
	
	const IOnlineSessionPtr Sessions = GetSessionInterface();
	if (!Sessions.IsValid())
	{
		FLIPPER_LOG(Warning, "sessions invalid");
		OnFlipperJoinSessionComplete.Broadcast(EOnJoinSessionCompleteResult::UnknownError);
		return;
	}
	
	JoinSessionCompleteHandle = Sessions->AddOnJoinSessionCompleteDelegate_Handle(OnJoinSessionComplete);
	
	const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	if (!Sessions->JoinSession(*LocalPlayer->GetPreferredUniqueNetId(), NAME_GameSession, SessionResult))
	{
		FLIPPER_LOG(Warning, "Error joining game session");
		Sessions->ClearOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteHandle);
		OnFlipperJoinSessionComplete.Broadcast(EOnJoinSessionCompleteResult::UnknownError);
	}
}

void UFlipperSessionSubsystem::OnJoinSessionCompleted(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	FLIPPER_LOG(Log, "Joining Game Session, result: %s", LexToString(Result));
	
	const IOnlineSessionPtr Sessions = GetSessionInterface();
	if (Sessions.IsValid())
	{
		Sessions->ClearOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteHandle);
	}
	
	OnFlipperJoinSessionComplete.Broadcast(Result);
}

void UFlipperSessionSubsystem::HandleNetworkFailure(UWorld* World, UNetDriver* NetDriver, ENetworkFailure::Type FailureType, const FString& ErrorString)
{
	// check if the failure is for the local PIE game instance
	if (!IsEventForThisGameInstance(World, NetDriver)) return;
	
	// only listen for client network failures so hosts aren't disconnected too
	if (!NetDriver || NetDriver->GetNetMode() != NM_Client) return;
	if (NetDriver->NetDriverName != NAME_GameNetDriver && NetDriver->NetDriverName != NAME_PendingNetDriver) return;
	
	// if the player is already leaving a failed network event is expected, so ignore it
	if (bIsLeaving) return;
	
	FLIPPER_LOG(Warning, "Network failure: %s - %s", ENetworkFailure::ToString(FailureType), *ErrorString);
	
	SetPendingErrorMessage(GetNetworkFailureMessage(FailureType, ErrorString));
	
	// delete the session the local user is in
	DestroySession();
}

void UFlipperSessionSubsystem::HandleTravelFailure(UWorld* World, ETravelFailure::Type FailureType, const FString& ErrorString)
{
	// check if the failure is for the local PIE game instance
	if (!IsEventForThisGameInstance(World, nullptr)) return;
	
	FLIPPER_LOG(Warning, "Travel failure: %s - %s", ETravelFailure::ToString(FailureType), *ErrorString);
	
	SetPendingErrorMessage(EFlipperSessionError::TravelFailed);
	
	DestroySession();
}

void UFlipperSessionSubsystem::HandlePostLoadMap(UWorld* LoadedWorld)
{
	// check if the event is for the local PIE game instance
	if (!IsEventForThisGameInstance(LoadedWorld, nullptr)) return;
	
	bIsLeaving = false;
}

bool UFlipperSessionSubsystem::TryTravelToCurrentSession()
{
	FLIPPER_LOG(Log, "Traveling to current session");
	
	const IOnlineSessionPtr Sessions = GetSessionInterface();
	if (!Sessions.IsValid()) return false;
	
	FString ConnectionString;
	if (!Sessions->GetResolvedConnectString(NAME_GameSession, ConnectionString))
	{
		FLIPPER_LOG(Error, "Resolved connection string failed to be retrieved: %s", *ConnectionString);
		return false;
	}
	
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	PlayerController->ClientTravel(ConnectionString, TRAVEL_Absolute);
	return true;
}

void UFlipperSessionSubsystem::LeaveSession()
{
	if (bIsLeaving) return;
	
	bIsLeaving = true;
	DestroySession();
}

FText UFlipperSessionSubsystem::UseAndClearPendingError()
{
	const FText TempMessage = PendingErrorMessage;
	PendingErrorMessage = FText::GetEmpty();
	return TempMessage;
}


IOnlineSessionPtr UFlipperSessionSubsystem::GetSessionInterface() const
{
	// get the session interface from the online subsystem
	// const IOnlineSubsystem* OnlineSubsystem = Online::GetSubsystem(GetWorld());
	// IOnlineSessionPtr FoundOnlineSessionInterface = OnlineSubsystem ? OnlineSubsystem->GetSessionInterface() : nullptr;
	//
	// return FoundOnlineSessionInterface.IsValid() ? FoundOnlineSessionInterface : nullptr;
	
	return Online::GetSessionInterface(GetWorld());
}

bool UFlipperSessionSubsystem::IsEventForThisGameInstance(const UWorld* World, const UNetDriver* NetDriver) const
{
	if (World)
	{
		return World->GetGameInstance() == GetGameInstance();
	}
	
	// if there's no world, presume we are joining it, and it doesn't exist yet
	// so get the owning game instance from the world context of the pending net driver
	// UnrealEngine.cpp:15179 (CallHandleDisconnectForFailure) does something similar to get the owning game instance of a pending world
	if (NetDriver && GEngine)
	{
		const FWorldContext* WorldContext = GEngine->GetWorldContextFromPendingNetGameNetDriver(NetDriver);
		if (WorldContext)
		{
			return WorldContext->OwningGameInstance == GetGameInstance();
		}
	}
	
	return false;
}

EFlipperSessionError UFlipperSessionSubsystem::GetNetworkFailureMessage(ENetworkFailure::Type FailureType, const FString& ErrorString)
{
	FLIPPER_LOG(Log, "GetNetworkFailureMessage for %s - %s", ENetworkFailure::ToString(FailureType), *ErrorString);
	
	// https://blueprinttocpp.com/node/flow-control/switch-on-int/
	// "You are missing a break; at the end of a case, so execution falls through to the next one"
	switch (FailureType)
	{
		// if the connection is lost or timed out
		case ENetworkFailure::ConnectionLost:
		case ENetworkFailure::ConnectionTimeout:
		case ENetworkFailure::FailureReceived:
			return EFlipperSessionError::ConnectionLost;
			
		case ENetworkFailure::PendingConnectionFailure:
			// if the error states the session is full
			if (ErrorString.Contains(TEXT("full"), ESearchCase::IgnoreCase))
				return EFlipperSessionError::SessionFull;
			return EFlipperSessionError::RejectedByHost;
			
		case ENetworkFailure::NetDriverAlreadyExists:
		case ENetworkFailure::NetDriverCreateFailure:
			return EFlipperSessionError::ConnectionFailed;
			
		case ENetworkFailure::OutdatedClient:
		case ENetworkFailure::OutdatedServer:
			return EFlipperSessionError::VersionMismatch;
			
		default:
			return EFlipperSessionError::UnknownNetworkError;
	}
}

void UFlipperSessionSubsystem::SetPendingErrorMessage(const EFlipperSessionError Error)
{
	// if the pending error message isn't empty, don't overwrite it
	if (!PendingErrorMessage.IsEmpty()) return;
	
	PendingErrorMessage = FlipperTypes::GetErrorText(Error);
}

void UFlipperSessionSubsystem::FinishDestroySession(bool bSuccessful)
{
	OnFlipperDestroySessionComplete.Broadcast(bSuccessful);
	
	// go to the main menu even if the session didn't successfully destroy
	// don't want the player to end up in a broken game
	if (bIsLeaving)
	{
		ReturnToMainMenu();
	}
}

void UFlipperSessionSubsystem::ReturnToMainMenu()
{
	const UFlipperGameInstance* GameInstance = Cast<UFlipperGameInstance>(GetGameInstance());
	if (!GameInstance || GameInstance->MenuLevel.IsNull())
	{
		FLIPPER_LOG(Error, "MenuLevel is not set on the game instance")
		bIsLeaving = false;
		return;
	}
	
	UGameplayStatics::OpenLevelBySoftObjectPtr(GetWorld(), GameInstance->MenuLevel);
}




