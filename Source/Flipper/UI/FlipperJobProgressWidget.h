// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Interactables/Instances/Standard/FlipperJobSite.h"
#include "FlipperJobProgressWidget.generated.h"

class UTextBlock;
class AFlipperJobSite;
class UImage;
/**
 * 
 */
UCLASS(Abstract)
class FLIPPER_API UFlipperJobProgressWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void Setup(AFlipperJobSite* JobSite);
	
protected:
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> CompletionProgressImage;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> JobNameTextBlock;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> JobProgressTextBlock;
	
	UPROPERTY(EditDefaultsOnly, Category = "Flipper|UI")
	TObjectPtr<UMaterialInstance> CompletionProgressMaterial;
	
	UPROPERTY(EditDefaultsOnly, Category = "Flipper|UI")
	FName MaterialFillProgressParameterName = FName("FillProgress");
	
	TWeakObjectPtr<UMaterialInstanceDynamic> CompletionProgressMaterialDynamic;
	
	TWeakObjectPtr<AFlipperJobSite> ObservedJobSite;
	
	UFUNCTION()
	void OnJobSiteCompleted(AFlipperJobSite* JobSite);
	
	UFUNCTION()
	void OnJobSiteProgressChanged(const FFlipperJobProgress& JobProgress);
	
	void RefreshVisuals();
};
