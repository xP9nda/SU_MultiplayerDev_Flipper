// Fill out your copyright notice in the Description page of Project Settings.


#include "FlipperJobsListWidget.h"

#include "FlipperJobProgressWidget.h"
#include "Components/VerticalBox.h"
#include "Core/FlipperGameState.h"

void UFlipperJobsListWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	const UWorld* World = GetWorld();
	if (!World) return;
	
	AFlipperGameState* FlipperGameState = World->GetGameState<AFlipperGameState>();
	if (!ensureMsgf(FlipperGameState, TEXT("Game state not valid on jobs list widget"))) return;
	
	// listen for when a job site is registered, and run through all already registered job sites
	FlipperGameState->OnJobSiteRegistered.AddUniqueDynamic(this, &UFlipperJobsListWidget::OnJobSiteRegistered);
	
	TArray<TWeakObjectPtr<AFlipperJobSite>> RegisteredJobSites = FlipperGameState->GetJobSites();
	for (auto RegisteredJobSite : RegisteredJobSites)
	{
		if (!RegisteredJobSite.IsValid()) continue;
		
		OnJobSiteRegistered(RegisteredJobSite.Get());
	}
}

void UFlipperJobsListWidget::NativeDestruct()
{
	const UWorld* World = GetWorld();
	if (!World) return;
	
	AFlipperGameState* FlipperGameState = World->GetGameState<AFlipperGameState>();
	if (!FlipperGameState) return;
	
	FlipperGameState->OnJobSiteRegistered.RemoveDynamic(this, &UFlipperJobsListWidget::OnJobSiteRegistered);
	
	Super::NativeDestruct();
}

void UFlipperJobsListWidget::OnJobSiteRegistered(AFlipperJobSite* JobSite)
{
	if (!JobSite) return;
	
	// listen for when the state is changed, calling OnJobSiteStateChanged when it is
	JobSite->OnJobSiteStateChanged.AddUniqueDynamic(this, &UFlipperJobsListWidget::OnJobSiteStateChanged);
}

void UFlipperJobsListWidget::OnJobSiteStateChanged(AFlipperJobSite* JobSite, EFlipperJobState JobState)
{
	// create a UFlipperJobProgressWidget widget when the job site's state becomes "in progress"
	// only creating a widget when the job site is it in progress, the widget handles cleaning itself up when job is completed
	if (!JobSite) return;
	if (JobState != EFlipperJobState::InProgress) return;
	if (!JobProgressWidgetClass) return;
	
	UFlipperJobProgressWidget* ProgressWidget = Cast<UFlipperJobProgressWidget>(CreateWidget<UUserWidget>(GetOwningPlayer(), JobProgressWidgetClass));
	ProgressWidget->Setup(JobSite);
	
	JobListVerticalBox->AddChildToVerticalBox(ProgressWidget);
}

