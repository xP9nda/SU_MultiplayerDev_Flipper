// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FlipperSessionTypes.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "OnlineSessionSettings.h"
#include "FlipperSessionSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FFlipperOnCreateSessionComplete, bool, bSuccessful);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FFlipperOnStartSessionComplete, bool, bSuccessful);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FFlipperOnDestroySessionComplete, bool, bSuccessful);
DECLARE_MULTICAST_DELEGATE_TwoParams(FFlipperOnFindSessionsComplete, const TArray<FOnlineSessionSearchResult>& SessionResults, bool bSuccessful);
DECLARE_MULTICAST_DELEGATE_OneParam(FFlipperOnJoinSessionComplete, EOnJoinSessionCompleteResult::Type Result);

/**
 * 
 */
UCLASS()
class FLIPPER_API UFlipperSessionSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	
	void CreateSession(int32 NumPublicConnections, bool bIsLanMatch);
	void StartSession();
	void DestroySession();
	void FindSessions(int32 MaxSearchResults, bool bIsLanQuery);
	void JoinGameSession(const FOnlineSessionSearchResult& SessionResult);
	
	bool TryTravelToCurrentSession();
	void LeaveSession();
	
	// menu calls this once when it opens which returns the stored message and clears it
	FText UseAndClearPendingError();
	
	bool IsLeaving() const { return bIsLeaving; }
	
	TSharedPtr<FOnlineSessionSearch>& GetLastSessionSearch() { return LastSessionSearch; }
	
	FFlipperOnCreateSessionComplete OnFlipperCreateSessionComplete;
	FFlipperOnStartSessionComplete OnFlipperStartSessionComplete;
	FFlipperOnDestroySessionComplete OnFlipperDestroySessionComplete;
	FFlipperOnFindSessionsComplete OnFlipperFindSessionsComplete;
	FFlipperOnJoinSessionComplete OnFlipperJoinSessionComplete;
	
protected:
	void OnCreateSessionCompleted(FName SessionName, bool bSuccessful);
	void OnStartSessionCompleted(FName SessionName, bool bSuccessful);
	void OnDestroySessionCompleted(FName SessionName, bool bSuccessful);
	void OnFindSessionsCompleted(bool bSuccessful);
	void OnJoinSessionCompleted(FName SessionName, EOnJoinSessionCompleteResult::Type Result);
	
	void HandleNetworkFailure(UWorld* World, UNetDriver* NetDriver, ENetworkFailure::Type FailureType, const FString& ErrorString);
	void HandleTravelFailure(UWorld* World, ETravelFailure::Type FailureType, const FString& ErrorString);
	void HandlePostLoadMap(UWorld* LoadedWorld);
	
private:
	IOnlineSessionPtr GetSessionInterface() const;
	
	// every client has its own game instance and copy of the subsystem but all bind into the same network failure
	// from the single global GEngine, meaning all clients hear every network failure, even if it's on another client,
	// so check the game instance locally against the game instance from the world
	bool IsEventForThisGameInstance(const UWorld* World, const UNetDriver* NetDriver) const;
	
	static EFlipperSessionError GetNetworkFailureMessage(ENetworkFailure::Type FailureType, const FString& ErrorString);
	void SetPendingErrorMessage(EFlipperSessionError Error);
	
	void FinishDestroySession(bool bSuccessful);
	void ReturnToMainMenu();
	
	bool bIsLeaving = false;
	FText PendingErrorMessage = FText::GetEmpty();
	
	TSharedPtr<FOnlineSessionSettings> LastSessionSettings;
	TSharedPtr<FOnlineSessionSearch> LastSessionSearch;
	
	FOnCreateSessionCompleteDelegate OnCreateSessionComplete;
	FDelegateHandle CreateSessionCompleteHandle;
	
	FOnStartSessionCompleteDelegate OnStartSessionComplete;
	FDelegateHandle StartSessionCompleteHandle;
	
	FOnDestroySessionCompleteDelegate OnDestroySessionComplete;
	FDelegateHandle DestroySessionCompleteHandle;
	
	FOnFindSessionsCompleteDelegate OnFindSessionsComplete;
	FDelegateHandle FindSessionsCompleteHandle;
	
	FOnJoinSessionCompleteDelegate OnJoinSessionComplete;
	FDelegateHandle JoinSessionCompleteHandle;
	
	FDelegateHandle NetworkFailureHandle;
	FDelegateHandle TravelFailureHandle;
	FDelegateHandle PostLoadMapHandle;
};
