// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OnlineSessionSettings.h"
#include "Blueprint/UserWidget.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "FlipperMenuSessionJoinableWidget.generated.h"

class UButton;
class UTextBlock;
/**
 * 
 */
UCLASS(Abstract)
class FLIPPER_API UFlipperMenuSessionJoinableWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void SetSessionResult(const FOnlineSessionSearchResult& InSessionResult);
	
protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

private:
	
	FOnlineSessionSearchResult SessionResult;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> SessionOwnerTextBlock;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> SessionPingTextBlock;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> SessionPlayersTextBlock;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> JoinSessionButton;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ButtonTextBlock;

	UFUNCTION()
	void OnJoinSessionButtonPressed();

	void OnJoinSessionComplete(EOnJoinSessionCompleteResult::Type Result);
};
