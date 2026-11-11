// Fill out your copyright notice in the Description page of Project Settings.


#include "FlipperJobProgressWidget.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Interactables/Instances/Standard/FlipperJobSite.h"

void UFlipperJobProgressWidget::Setup(AFlipperJobSite* JobSite)
{
	if (!JobSite) return;
	
	ObservedJobSite = JobSite;
	
	ObservedJobSite->OnJobSiteCompleted.AddDynamic(this, &UFlipperJobProgressWidget::OnJobSiteCompleted);
	ObservedJobSite->OnJobSiteProgressChanged.AddDynamic(this, &UFlipperJobProgressWidget::OnJobSiteProgressChanged);
	
	if (JobNameTextBlock)
	{
		JobNameTextBlock->SetText(JobSite->GetJobName());
	}
}

void UFlipperJobProgressWidget::NativePreConstruct()
{
	Super::NativePreConstruct();
}

void UFlipperJobProgressWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	if (CompletionProgressMaterial)
	{
		CompletionProgressMaterialDynamic = UMaterialInstanceDynamic::Create(CompletionProgressMaterial, this);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("CompletionProgressMaterial is null on: %s"), *this->GetName());
	}
	
	if (CompletionProgressMaterialDynamic.IsValid() && CompletionProgressImage)
	{
		CompletionProgressImage->SetBrushFromMaterial(CompletionProgressMaterialDynamic.Get());
	}
	
	RefreshVisuals();
}

void UFlipperJobProgressWidget::NativeDestruct()
{
	if (ObservedJobSite.IsValid())
	{
		ObservedJobSite->OnJobSiteCompleted.RemoveDynamic(this, &UFlipperJobProgressWidget::OnJobSiteCompleted);
		ObservedJobSite->OnJobSiteProgressChanged.RemoveDynamic(this, &UFlipperJobProgressWidget::OnJobSiteProgressChanged);
	}
	
	Super::NativeDestruct();
}

void UFlipperJobProgressWidget::RefreshVisuals()
{
	if (!ObservedJobSite.IsValid())
	{
		RemoveFromParent();
	}
	
	TArray<FText> DescriptionLines;
	
	const int32 NumberPaintableMeshes = ObservedJobSite->GetNumberPaintableMeshes();
	const int32 NumberPickupMeshes = ObservedJobSite->GetNumberPickupMeshes();
	const FFlipperJobProgress JobProgress = ObservedJobSite->GetJobProgress();
	const int32 CompletionPayout = ObservedJobSite->GetCompletionPayout();
	
	const int32 TotalCompletionSteps = NumberPaintableMeshes + NumberPickupMeshes;
	const int32 CompletedJobSteps = JobProgress.CompletedPaintableMeshes + JobProgress.CompletedPickupMeshes;
	
	const float TotalProgress = TotalCompletionSteps > 0 ? FMath::Clamp(static_cast<float>(CompletedJobSteps) / static_cast<float>(TotalCompletionSteps), 0.0f, 1.0f) : 0;
	DescriptionLines.Add(FText::Format(FText::FromString("{0}% Complete:"), TotalProgress * 100.0f));
	
	if (CompletionProgressMaterialDynamic.IsValid())
	{
		CompletionProgressMaterialDynamic->SetScalarParameterValue(MaterialFillProgressParameterName, TotalProgress);
	}
	
	if (NumberPaintableMeshes > 0)
	{
		const int32 Progress = FMath::RoundToInt(100.0f * JobProgress.CompletedPaintableMeshes / NumberPaintableMeshes);
		DescriptionLines.Add(FText::Format(FText::FromString("  - {0}/{1} painted ({2}%)"), JobProgress.CompletedPaintableMeshes, NumberPaintableMeshes, Progress));
	}
	
	if (NumberPickupMeshes > 0)
	{
		const int32 Progress = FMath::RoundToInt(100.0f * JobProgress.CompletedPickupMeshes / NumberPickupMeshes);
		DescriptionLines.Add(FText::Format(FText::FromString("  - {0}/{1} trash picked up ({2}%)"), JobProgress.CompletedPickupMeshes, NumberPickupMeshes, Progress));
	}
	
	const FCulturePtr USCulture = FInternationalization::Get().GetCulture(TEXT("en-US")); // strip the "US" from "US$x.xx"
	const FText FormattedCompletionPayout = FText::AsCurrencyBase(CompletionPayout, TEXT("USD"), USCulture);
	DescriptionLines.Add(FText::Format(FText::FromString("\n({0} completion bonus)"), FormattedCompletionPayout));
	
	if (JobProgressTextBlock)
	{
		JobProgressTextBlock->SetText(FText::Join(FText::FromString("\n"), DescriptionLines));
	}
	
	if (ObservedJobSite->GetJobState() == EFlipperJobState::Completed)
	{
		RemoveFromParent();
	}
}

void UFlipperJobProgressWidget::OnJobSiteCompleted(AFlipperJobSite* JobSite)
{
	RefreshVisuals();
}

void UFlipperJobProgressWidget::OnJobSiteProgressChanged(const FFlipperJobProgress& JobProgress)
{
	RefreshVisuals();
}
