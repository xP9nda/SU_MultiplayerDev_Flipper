// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interactables/FlipperBaseInteractable.h"
#include "FlipperJobSite.generated.h"

class AFlipperUnitPickupMesh;
class AFlipperPaintableMesh;

UENUM()
enum class EFlipperJobState : uint8
{
	NotStarted,
	InProgress,
	Completed
};

USTRUCT()
struct FFlipperJobProgress
{
	GENERATED_BODY()
	
	UPROPERTY()
	uint8 CompletedPaintableMeshes = 0;
	
	UPROPERTY()
	uint8 CompletedPickupMeshes = 0;	
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnJobSiteStateChanged, AFlipperJobSite*, JobSite, EFlipperJobState, JobState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnJobSiteCompleted, AFlipperJobSite*, JobSite);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnJobSiteProgressChanged, const FFlipperJobProgress&, JobProgress);

UCLASS()
class FLIPPER_API AFlipperJobSite : public AFlipperBaseInteractable
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AFlipperJobSite();
	
	EFlipperJobState GetJobState() const { return JobState; }
	FFlipperJobProgress GetJobProgress() const { return JobProgress; }
	int32 GetCompletionPayout() const { return CompletionPayout; }
	FText GetJobName() const { return JobName; }
	int32 GetNumberPaintableMeshes() const { return NumberPaintableMeshes; }
	int32 GetNumberPickupMeshes() const { return NumberPickupMeshes; }
	
	FOnJobSiteStateChanged OnJobSiteStateChanged;
	FOnJobSiteCompleted OnJobSiteCompleted;
	FOnJobSiteProgressChanged OnJobSiteProgressChanged;
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void Interact_Implementation(AActor* Interactor) override;
	virtual FFlipperInteractText GetInteractText(AActor* Interactor) override;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// BlueprintReadWrite so editor tool can set these values
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Flipper|Job")
	TArray<TObjectPtr<AFlipperPaintableMesh>> PaintableMeshes;
	
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Flipper|Job")
	TArray<TObjectPtr<AFlipperUnitPickupMesh>> PickupMeshes;
	
private:
	UPROPERTY(EditInstanceOnly, Category = "Flipper|Job")
	FText JobName;
	
	
	UPROPERTY(EditAnywhere, Category = "Flipper|Job", meta = (ClampMin = "0"))
	int32 CompletionPayout = 100;
	
	UPROPERTY(ReplicatedUsing = OnRep_JobState)
	EFlipperJobState JobState = EFlipperJobState::NotStarted;
	
	UPROPERTY(ReplicatedUsing = OnRep_JobProgress)
	FFlipperJobProgress JobProgress;
	
	int32 NumberPaintableMeshes = 0;
	int32 NumberPickupMeshes = 0;
	
	void SetJobState(EFlipperJobState NewJobState);
	void UpdateCompletionStatus();
	void SetInteractablesCanInteract(bool bNewCanInteract);

	UFUNCTION()
	void OnRep_JobState();
	
	UFUNCTION()
	void OnRep_JobProgress();
	
	UFUNCTION()
	void HandlePaintableMeshCompleted(AFlipperPaintableMesh* Mesh, APawn* FinishedBy);
	UFUNCTION()
	void HandlePickupMeshCompleted(AFlipperUnitPickupMesh* Mesh, APawn* FinishedBy);
};
