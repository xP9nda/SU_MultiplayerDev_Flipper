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
	
	const TArray<TWeakObjectPtr<AFlipperJobSite>> RegisteredJobSites = FlipperGameState->GetJobSites();
	for (auto RegisteredJobSite : RegisteredJobSites)
	{
		if (!RegisteredJobSite.IsValid()) continue;
		
		OnJobSiteRegistered(RegisteredJobSite.Get());
	}
}

void UFlipperJobsListWidget::NativeDestruct()
{
	const UWorld* World = GetWorld();
	AFlipperGameState* FlipperGameState = World ? World->GetGameState<AFlipperGameState>() : nullptr;
	if (FlipperGameState)
	{
		FlipperGameState->OnJobSiteRegistered.RemoveDynamic(this, &UFlipperJobsListWidget::OnJobSiteRegistered);
	}
	
	Super::NativeDestruct();
}

void UFlipperJobsListWidget::OnJobSiteRegistered(AFlipperJobSite* JobSite)
{
	if (!JobSite) return;
	
	// listen for when the state is changed, calling OnJobSiteStateChanged when it is
	JobSite->OnJobSiteStateChanged.AddUniqueDynamic(this, &UFlipperJobsListWidget::OnJobSiteStateChanged);
	
	// if the job site state is already in progress OR complete, manually call the delegate callback
	// so that late joiners see the UI immediately OR they see that all sites are done
	if (JobSite->GetJobState() != EFlipperJobState::NotStarted)
	{
		OnJobSiteStateChanged(JobSite, JobSite->GetJobState());
	}
}

void UFlipperJobsListWidget::OnJobSiteStateChanged(AFlipperJobSite* JobSite, const EFlipperJobState JobState)
{
	if (!JobSite) return;
	
	// if the job site has changed to completed, check all other job sites
	// if all are complete, instantiate a JobsCompleteWidgetClass in the job progress list
	if (JobState == EFlipperJobState::Completed)
	{
		const UWorld* World = GetWorld();
		AFlipperGameState* FlipperGameState = World ? World->GetGameState<AFlipperGameState>() : nullptr;
		if (!FlipperGameState) return;
		
		const auto JobSites = FlipperGameState->GetJobSites();
		// no job sites should not count as everything has been completed
		bool bAllJobSitesComplete = !JobSites.IsEmpty();
		
		for (auto Element : JobSites)
		{
			if (!Element.IsValid() || Element->GetJobState() != EFlipperJobState::Completed)
			{
				bAllJobSitesComplete = false;
				break;
			}
		}
		
		// if all job sites are complete and the completion widget hasn't yet been made
		if (bAllJobSitesComplete && !IsValid(JobsCompletionWidget))
		{
			JobsCompletionWidget = CreateWidget<UUserWidget>(GetOwningPlayer(), JobsCompleteWidgetClass);
			if (JobsCompletionWidget)
			{
				JobListVerticalBox->AddChild(JobsCompletionWidget);
			}
		}
	}
	
	// create a UFlipperJobProgressWidget widget when the job site's state becomes "in progress"
	// only creating a widget when the job site is it in progress, the widget handles cleaning itself up when job is completed
	if (JobState != EFlipperJobState::InProgress) return;
	if (!JobProgressWidgetClass) return;
	
	UFlipperJobProgressWidget* ProgressWidget = Cast<UFlipperJobProgressWidget>(CreateWidget<UUserWidget>(GetOwningPlayer(), JobProgressWidgetClass));
	if (ProgressWidget)
	{
		ProgressWidget->Setup(JobSite);
		JobListVerticalBox->AddChildToVerticalBox(ProgressWidget);
	}
}

