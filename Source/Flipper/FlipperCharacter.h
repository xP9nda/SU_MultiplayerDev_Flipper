// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "FlipperCharacter.generated.h"

class UFlipperToolComponent;
class UFlipperInteractComponent;
class UInputComponent;
class USkeletalMeshComponent;
class UCameraComponent;
class UInputAction;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

/**
 *  A basic first person character
 */
UCLASS(abstract)
class AFlipperCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Pawn mesh: first person view (arms; seen only by self) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* FirstPersonMesh;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FirstPersonCameraComponent;
	
	// Interaction - todo document 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UFlipperInteractComponent> InteractionComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UFlipperToolComponent> ToolComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components|Tools", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> ToolMeshComponentFirstPerson;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components|Tools", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> ToolMeshComponentThirdPerson;
	
	// Usable Units
	// commented out in favour of adding them in the blueprint
	// adding them in bp leaves it more modular and designer friendly, no c++ change needed to add usable unit components
	// this works as the properties exposed on the component drive the kind of utility it has
	// UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	// TObjectPtr<UFlipperUsableUnitComponent> PaintComponent;
	//
	// UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	// TObjectPtr<UFlipperUsableUnitComponent> WallpaperComponent;

protected:

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, Category ="Input")
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, Category ="Input")
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, Category ="Input")
	class UInputAction* LookAction;

	/** Mouse Look Input Action */
	UPROPERTY(EditAnywhere, Category ="Input")
	class UInputAction* MouseLookAction;
	
	// Interaction - todo document
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* InteractAction;
	
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* ToolChangeScrollForwardAction;
	
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* ToolChangeScrollBackwardAction;
	
	UPROPERTY(EditAnywhere, Category="Input|Toolbar")
	UInputAction* ToolbarTool1Action;
	
	UPROPERTY(EditAnywhere, Category="Input|Toolbar")
	UInputAction* ToolbarTool2Action;
	
	UPROPERTY(EditAnywhere, Category="Input|Toolbar")
	UInputAction* ToolbarTool3Action;
	
public:
	AFlipperCharacter();

protected:

	/** Called from Input Actions for movement input */
	void MoveInput(const FInputActionValue& Value);

	/** Called from Input Actions for looking input */
	void LookInput(const FInputActionValue& Value);

	/** Handles aim inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoAim(float Yaw, float Pitch);

	/** Handles move inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoMove(float Right, float Forward);

	/** Handles jump start inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoJumpStart();

	/** Handles jump end inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoJumpEnd();
	
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoToolScrollForward();
	
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoToolScrollBackward();
	
	UFUNCTION(BlueprintCallable, Category="Input|Toolbar")
	virtual void DoToolbarTool1();
	
	UFUNCTION(BlueprintCallable, Category="Input|Toolbar")
	virtual void DoToolbarTool2();
	
	UFUNCTION(BlueprintCallable, Category="Input|Toolbar")
	virtual void DoToolbarTool3();
	
	UFUNCTION()
	void HandleToolChanged(FGameplayTag NewTool);

protected:

	/** Set up input action bindings */
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	
	virtual void Tick(float DeltaTime) override;
	

public:
	virtual void BeginPlay() override;

	/** Returns the first person mesh **/
	USkeletalMeshComponent* GetFirstPersonMesh() const { return FirstPersonMesh; }

	/** Returns first person camera component **/
	UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }
};

