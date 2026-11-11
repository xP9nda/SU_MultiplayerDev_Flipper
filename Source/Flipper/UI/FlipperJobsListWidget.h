// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Interactables/Instances/Standard/FlipperJobSite.h"
#include "FlipperJobsListWidget.generated.h"

class UFlipperJobProgressWidget;
class UVerticalBox;
/**
 * 
 */
UCLASS(Abstract)
class FLIPPER_API UFlipperJobsListWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

private:
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVerticalBox> JobListVerticalBox;

	UPROPERTY(EditDefaultsOnly, Category = "Flipper|UI")
	TSubclassOf<UFlipperJobProgressWidget> JobProgressWidgetClass; 
	
	UFUNCTION()
	void OnJobSiteRegistered(AFlipperJobSite* JobSite);
	
	UFUNCTION()
	void OnJobSiteStateChanged(AFlipperJobSite* JobSite, EFlipperJobState JobState);

};
