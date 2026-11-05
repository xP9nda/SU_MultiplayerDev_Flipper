// Fill out your copyright notice in the Description page of Project Settings.


#include "FlipperToolComponent.h"

#include "Net/UnrealNetwork.h"


// Sets default values for this component's properties
UFlipperToolComponent::UFlipperToolComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	
	SetIsReplicatedByDefault(true);

	// ...
}

void UFlipperToolComponent::RequestSetTool(FGameplayTag NewTool)
{
	// validate locally that the tool is available
	if (!AvailableTools.Contains(NewTool)) return;
	if (CurrentTool == NewTool) return;
	
	// immediately broadcast a prediction of the current tool so that the client visually updates immediately
	// because CurrentTool is COND_SkipOwner, the server will not overwrite this value, so only RejectToolChange can undo a change
	CurrentTool = NewTool;
	OnToolChanged.Broadcast(CurrentTool);
	
	// send a message to the server to set the current tool to the new tool
	Server_SetCurrentTool(NewTool);
}

void UFlipperToolComponent::CycleTool(const int32 Direction)
{
	if (AvailableTools.IsEmpty()) return;
	
	const int32 CurrentToolIndex = AvailableTools.IndexOfByKey(CurrentTool);
	// IndexOfByKey returns INDEX_NONE when the value isn't found, so this checks both for whether AvailableTools.Contains(CurrentTool) and the current tool index lookup
	if (CurrentToolIndex == INDEX_NONE)
	{
		RequestSetTool(AvailableTools[0]);
		return;
	}
	
	const int32 NumTools = AvailableTools.Num();
	const int32 NewToolIndex = (CurrentToolIndex + Direction + NumTools) % NumTools;
	RequestSetTool(AvailableTools[NewToolIndex]);
}


void UFlipperToolComponent::Server_SetCurrentTool_Implementation(FGameplayTag NewTool)
{
	// validate that the tool exists, if not, send a message to the client rejecting the request
	if (!AvailableTools.Contains(NewTool))
	{
		Client_RejectToolChange(NewTool, CurrentTool);
		return;
	}
	
	// on a listen server host RequestSetTool already set the value locally and broadcast the change,
	// so don't broadcast twice
	if (NewTool == CurrentTool) return;
	
	// update the new tool and listen server host update
	CurrentTool = NewTool;
	OnRep_CurrentTool();
}

void UFlipperToolComponent::Client_RejectToolChange_Implementation(FGameplayTag RejectedTool, FGameplayTag AuthoritativeTool)
{
	// only reject the change if the current tool that's visually shown is wrong
	// if the player already quickly moved to another tool, the request may still be incoming and will get its own accept/rejection
	// reverting here would desync player tool because COND_SkipOwner means the server won't correct it afterwards
	if (CurrentTool != RejectedTool) return;
	
	CurrentTool = AuthoritativeTool;
	OnToolChanged.Broadcast(CurrentTool);
}


void UFlipperToolComponent::OnRep_CurrentTool()
{
	// runs on other clients when the player's tool replicates to them
	// this doesn't run on the owning client because of SkipOwner, broadcasts are done manually
	OnToolChanged.Broadcast(CurrentTool);
}

// Called when the game starts
void UFlipperToolComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
	// set the default tool to the first one available
	// CurrentTool is COND_SkipOwner so the owning client never receives a starting value from the server
	// picking [0] everywhere means that everything agrees on the starting tool without unnecessarily replicating anything
	if (!CurrentTool.IsValid() && !AvailableTools.IsEmpty())
	{
		CurrentTool = AvailableTools[0];
	}
	
	// broadcast a message for listeners that are already bound (i.e. HUD)
	OnToolChanged.Broadcast(CurrentTool);
}


void UFlipperToolComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	// everyone except the owner needs this to see what tool a player is holding
	// the owner is skipped because it predicts the tool locally and replication would snap back to old values if tools are changed quickly
	DOREPLIFETIME_CONDITION(UFlipperToolComponent, CurrentTool, COND_SkipOwner)
}

