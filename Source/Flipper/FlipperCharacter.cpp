// Copyright Epic Games, Inc. All Rights Reserved.

#include "FlipperCharacter.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Flipper.h"
#include "Cheats/FlipperCheatManager.h"
#include "Components/FlipperInteractComponent.h"
#include "Components/FlipperToolComponent.h"
#include "Core/FlipperPlayerState.h"

AFlipperCharacter::AFlipperCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);
	
	// Create the first person mesh that will be viewed only by this character's owner
	FirstPersonMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("First Person Mesh"));

	FirstPersonMesh->SetupAttachment(GetMesh());
	FirstPersonMesh->SetOnlyOwnerSee(true);
	FirstPersonMesh->FirstPersonPrimitiveType = EFirstPersonPrimitiveType::FirstPerson;
	FirstPersonMesh->SetCollisionProfileName(FName("NoCollision"));

	// Create the Camera Component	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("First Person Camera"));
	FirstPersonCameraComponent->SetupAttachment(FirstPersonMesh, FName("head"));
	FirstPersonCameraComponent->SetRelativeLocationAndRotation(FVector(-2.8f, 5.89f, 0.0f), FRotator(0.0f, 90.0f, -90.0f));
	FirstPersonCameraComponent->bUsePawnControlRotation = true;
	FirstPersonCameraComponent->bEnableFirstPersonFieldOfView = true;
	FirstPersonCameraComponent->bEnableFirstPersonScale = true;
	FirstPersonCameraComponent->FirstPersonFieldOfView = 70.0f;
	FirstPersonCameraComponent->FirstPersonScale = 0.6f;

	// tool meshes
	// first person, using SetOnlyOwnerSee and FirstPersonMesh means this is just the arms (as seen locally in first person_
	ToolMeshComponentFirstPerson = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ToolMesh Component FirstPerson"));
	ToolMeshComponentFirstPerson->SetupAttachment(FirstPersonMesh, FName("ToolSocket")); // use specifically the first person mesh set up above (main mesh from GetMesh() so that's used in ThirdPerson tool mesh)
	ToolMeshComponentFirstPerson->SetOnlyOwnerSee(true);
	ToolMeshComponentFirstPerson->FirstPersonPrimitiveType = EFirstPersonPrimitiveType::FirstPerson;
	ToolMeshComponentFirstPerson->SetCollisionProfileName(FName("NoCollision"));
	ToolMeshComponentFirstPerson->SetCastShadow(false);
	
	// full body mesh, everyone except the owner sees this version - for other clients to see the held tool (via SetOwnerNoSee so owner doesn't see this version)
	ToolMeshComponentThirdPerson = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ToolMesh Component ThirdPerson"));
	ToolMeshComponentThirdPerson->SetupAttachment(GetMesh(), FName("ToolSocket")); // GetMesh() used as described above
	ToolMeshComponentThirdPerson->SetOwnerNoSee(true);
	ToolMeshComponentThirdPerson->FirstPersonPrimitiveType = EFirstPersonPrimitiveType::WorldSpaceRepresentation;
	ToolMeshComponentThirdPerson->SetCollisionProfileName(FName("NoCollision"));
	
	// configure the character comps
	GetMesh()->SetOwnerNoSee(true);
	GetMesh()->FirstPersonPrimitiveType = EFirstPersonPrimitiveType::WorldSpaceRepresentation;

	GetCapsuleComponent()->SetCapsuleSize(34.0f, 96.0f);

	// Configure character movement
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;
	GetCharacterMovement()->AirControl = 0.5f;
	
	// Interaction  - todo document
	InteractionComponent = CreateDefaultSubobject<UFlipperInteractComponent>(TEXT("InteractionComponent"));
	ToolComponent = CreateDefaultSubobject<UFlipperToolComponent>(TEXT("ToolComponent"));
	
	// Usable Units
	// commented out in favour of adding them in the blueprint
	// adding them in bp leaves it more modular and designer friendly, no c++ change needed to add usable unit components
	// this works as the properties exposed on the component drive the kind of utility it has
	// // - Paint
	// PaintComponent = CreateDefaultSubobject<UFlipperUsableUnitComponent>(TEXT("PaintComponent"));
	// // - Wallpaper
	// WallpaperComponent = CreateDefaultSubobject<UFlipperUsableUnitComponent>(TEXT("WallpaperComponent")); 
}

void AFlipperCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{	
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &AFlipperCharacter::DoJumpStart);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &AFlipperCharacter::DoJumpEnd);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AFlipperCharacter::MoveInput);

		// Looking/Aiming
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AFlipperCharacter::LookInput);
		EnhancedInputComponent->BindAction(MouseLookAction, ETriggerEvent::Triggered, this, &AFlipperCharacter::LookInput);
		
		// Interaction - todo document
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Triggered, InteractionComponent.Get(), &UFlipperInteractComponent::AttemptInteract);
		
		// Tool Switching
		EnhancedInputComponent->BindAction(ToolChangeScrollForwardAction, ETriggerEvent::Started, this, &AFlipperCharacter::DoToolScrollForward);
		EnhancedInputComponent->BindAction(ToolChangeScrollBackwardAction, ETriggerEvent::Started, this, &AFlipperCharacter::DoToolScrollBackward);
	
		EnhancedInputComponent->BindAction(ToolbarTool1Action, ETriggerEvent::Started, this, &AFlipperCharacter::DoToolbarTool1);
		EnhancedInputComponent->BindAction(ToolbarTool2Action, ETriggerEvent::Started, this, &AFlipperCharacter::DoToolbarTool2);
		EnhancedInputComponent->BindAction(ToolbarTool3Action, ETriggerEvent::Started, this, &AFlipperCharacter::DoToolbarTool3);
		EnhancedInputComponent->BindAction(ToolbarTool4Action, ETriggerEvent::Started, this, &AFlipperCharacter::DoToolbarTool4);
	}
	else
	{
		UE_LOG(LogFlipper, Error, TEXT("'%s' Failed to find an Enhanced Input Component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}


void AFlipperCharacter::MoveInput(const FInputActionValue& Value)
{
	// get the Vector2D move axis
	const FVector2D MovementVector = Value.Get<FVector2D>();

	// pass the axis values to the move input
	DoMove(MovementVector.X, MovementVector.Y);

}

void AFlipperCharacter::LookInput(const FInputActionValue& Value)
{
	// get the Vector2D look axis
	const FVector2D LookAxisVector = Value.Get<FVector2D>();

	// pass the axis values to the aim input
	DoAim(LookAxisVector.X, LookAxisVector.Y);

}

void AFlipperCharacter::DoAim(float Yaw, float Pitch)
{
	if (GetController())
	{
		// pass the rotation inputs
		AddControllerYawInput(Yaw);
		AddControllerPitchInput(Pitch);
	}
}

void AFlipperCharacter::DoMove(float Right, float Forward)
{
	if (GetController())
	{
		// pass the move inputs
		AddMovementInput(GetActorRightVector(), Right);
		AddMovementInput(GetActorForwardVector(), Forward);
	}
}

void AFlipperCharacter::DoJumpStart()
{
	// pass Jump to the character
	Jump();
}

void AFlipperCharacter::DoJumpEnd()
{
	// pass StopJumping to the character
	StopJumping();
}

void AFlipperCharacter::DoToolScrollForward()
{
	if (!ToolComponent) return;
	ToolComponent->CycleTool(1);
}

void AFlipperCharacter::DoToolScrollBackward()
{
	if (!ToolComponent) return;
	ToolComponent->CycleTool(-1);
}

void AFlipperCharacter::DoToolbarTool1()
{
	if (!ToolComponent) return;
	ToolComponent->SetToolIndex(0);
}

void AFlipperCharacter::DoToolbarTool2()
{
	if (!ToolComponent) return;
	ToolComponent->SetToolIndex(1);
}

void AFlipperCharacter::DoToolbarTool3()
{
	if (!ToolComponent) return;
	ToolComponent->SetToolIndex(2);
}

void AFlipperCharacter::DoToolbarTool4()
{
	if (!ToolComponent) return;
	ToolComponent->SetToolIndex(3);
}

void AFlipperCharacter::HandleToolChanged(FGameplayTag NewTool)
{
	UStaticMesh* ToolMesh = ToolComponent ? ToolComponent->GetMeshForTool(NewTool) : nullptr;
	
	// if ToolMesh is nullptr, still set the static meshes to that nullptr value to clear the meshes and show an empty hand
	ToolMeshComponentFirstPerson->SetStaticMesh(ToolMesh);
	ToolMeshComponentThirdPerson->SetStaticMesh(ToolMesh);
}


static FString RoleToString(ENetRole Role)
{
	switch (Role)
	{
	case ROLE_Authority:       return TEXT("Authority");
	case ROLE_AutonomousProxy: return TEXT("AutonomousProxy");
	case ROLE_SimulatedProxy:  return TEXT("SimulatedProxy");
	default:                 return TEXT("None");
	}
}

void AFlipperCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

#if !UE_BUILD_SHIPPING
	
	if (!GEngine) return;

	const APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	const UFlipperCheatManager* FlipperCheatManager = PlayerController ? Cast<UFlipperCheatManager>(PlayerController->CheatManager) : nullptr;
	if (!FlipperCheatManager) return;

	// net debugging
	if (FlipperCheatManager->IsNetDebuggingEnabled())
	{
		const uint64 Key = (uint64)GetUniqueID();
		const FString Line = FString::Printf(
			TEXT("%s | Local: %s | Remote: %s | HasAuthority: %s"),
			*GetName(), *RoleToString(GetLocalRole()), *RoleToString(GetRemoteRole()),
			HasAuthority() ? TEXT("true") : TEXT("false"));

		FColor Colour = FColor::Yellow;
		if (GetLocalRole() == ROLE_Authority)            Colour = FColor::Green;
		else if (GetLocalRole() == ROLE_AutonomousProxy) Colour = FColor::Cyan;

		GEngine->AddOnScreenDebugMessage(Key, 2.0f, Colour, Line);
	}
	
	// player state debugging
	if (FlipperCheatManager->IsPlayerStateDebuggingEnabled())
	{
		const AFlipperPlayerState* FlipperPlayerState = GetPlayerState<AFlipperPlayerState>();
		if (!FlipperPlayerState) return;
		
		const FFlipperStepsContribution& PlayerSurfaceContribution = FlipperPlayerState->GetSurfaceContribution();
		
		const uint64 Key = (uint64)GetUniqueID();
		const FString Line = FString::Printf(
			TEXT("%s | FUNDS EARNED: %d | FUNDS SPENT: %d | SURFACE CONTRIBUTION: (%d correct / %d incorrect)"),
			*GetName(), FlipperPlayerState->GetFundsEarned(), FlipperPlayerState->GetFundsSpent(), PlayerSurfaceContribution.CorrectSteps, PlayerSurfaceContribution.IncorrectSteps);

		GEngine->AddOnScreenDebugMessage(Key, 2.0f, FColor::Green, Line);
	}
	
#endif
}

void AFlipperCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	if (!ToolComponent) return;
	
	ToolComponent->OnToolChanged.AddDynamic(this, &AFlipperCharacter::HandleToolChanged);
	
	// apply the current tool immediately
	HandleToolChanged(ToolComponent->GetCurrentTool());
}
