// Fill out your copyright notice in the Description page of Project Settings.


#include "FlipperUnitProvider.h"

#include "Components/FlipperToolComponent.h"
#include "Components/FlipperUsableUnitComponent.h"
#include "Core/FlipperGameInstance.h"
#include "Core/FlipperGameState.h"
#include "Core/FlipperPlayerState.h"
#include "DataAssets/FlipperSurfaceFinishData.h"


// Sets default values
AFlipperUnitProvider::AFlipperUnitProvider()
{
	bReplicates = true;

	FillStaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FillStaticMeshComponent"));
}

void AFlipperUnitProvider::SetupMaterial()
{
	if (FillMaterial)
	{
		FillMaterialDynamic = UMaterialInstanceDynamic::Create(FillMaterial, this);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("FillMaterial is null on: %s"), *this->GetName());
	}
	
	if (FillStaticMeshComponent && FillMaterialDynamic)
	{
		FillStaticMeshComponent->SetMaterial(0, FillMaterialDynamic);
		
		// ensure default material parameters are applied
		const UFlipperSurfaceFinishData* SurfaceFinishData = GetSurfaceData();
		if (SurfaceFinishData)
		{
			if (!ProvidedFinish.IsValid()) return; // cancel if there's no provided finish set
			
			// get the starting provided finish from the game instance data asset and ensure the material is set up with that information
			const FFlipperSurfaceFinish* ProvidedSurfaceFinish = SurfaceFinishData->GetFinish(ProvidedFinish);
			if (ProvidedSurfaceFinish)
			{
				// update the colour and texture with whatever surface finish is given out by this unit provider
				FillMaterialDynamic->SetVectorParameterValue(MaterialProvidedColourParameterName, ProvidedSurfaceFinish->SurfaceColour);
			
				UTexture* ProvidedTexture = SurfaceFinishData->GetMaterialTexture(ProvidedSurfaceFinish->MaterialTexture);
				if (ProvidedTexture)
				{
					FillMaterialDynamic->SetTextureParameterValue(MaterialProvidedTextureParameterName, ProvidedTexture);
				}
			}
		}
	}
}

// Called when the game starts or when spawned
void AFlipperUnitProvider::BeginPlay()
{
	Super::BeginPlay();
	
	// set up the material before authority check
	// so that each client shows the material on its own copy of the actor
	SetupMaterial();
}

EFlipperPurchaseResult AFlipperUnitProvider::EvaluatePurchaseInformation(AActor* Interactor, FFlipperPurchaseInformation& PurchaseInfo) const
{
	if (!ProvidedFinish.IsValid()) return EFlipperPurchaseResult::InvalidFinish;
	
	const UFlipperSurfaceFinishData* SurfaceFinishData = GetSurfaceData();
	PurchaseInfo.ProvidedSurfaceFinish = SurfaceFinishData ? SurfaceFinishData->GetFinish(ProvidedFinish) : nullptr;
	if (!PurchaseInfo.ProvidedSurfaceFinish) return EFlipperPurchaseResult::InvalidFinish;
	
	PurchaseInfo.UnitsComponent = GetActiveUnitsComponent(Interactor);
	if (!PurchaseInfo.UnitsComponent) return EFlipperPurchaseResult::WrongTool;
	
	// don't allow swapping finishes while holding units of a different finish
	if (PurchaseInfo.UnitsComponent->GetUsableUnits() > 0 && PurchaseInfo.UnitsComponent->GetCurrentFinish() != ProvidedFinish) return EFlipperPurchaseResult::DifferentFinishHeld;
	
	// fill whatever space is left and don't allow filling an already full usable unit component
	PurchaseInfo.UnitsToPurchase = PurchaseInfo.UnitsComponent->GetMaxUsableUnits() - PurchaseInfo.UnitsComponent->GetUsableUnits();
	if (PurchaseInfo.UnitsToPurchase == 0) return EFlipperPurchaseResult::Full;
	
	PurchaseInfo.TotalCost = PurchaseInfo.ProvidedSurfaceFinish->UnitPrice * static_cast<int32>(PurchaseInfo.UnitsToPurchase);
	return EFlipperPurchaseResult::Success;
}

void AFlipperUnitProvider::Interact_Implementation(AActor* Interactor)
{
	if (!HasAuthority()) return;
	if (!ProvidedFinish.IsValid()) return;
	
	AFlipperGameState* FlipperGameState = GetFlipperGameState();
	if (!FlipperGameState) return;
	
	FFlipperPurchaseInformation PurchaseInfo;
	if (EvaluatePurchaseInformation(Interactor, PurchaseInfo) != EFlipperPurchaseResult::Success) return;
	
	AFlipperPlayerState* InteractorPlayerState = GetInteractorPlayerState(Interactor);
	
	// if the team can't afford it don't change anything
	if (PurchaseInfo.TotalCost > 0 && !FlipperGameState->TrySpendFunds(PurchaseInfo.TotalCost, InteractorPlayerState)) return;
	
	// set the finish BEFORE adding units, as TrySetCurrentFinish reads whether UsableUnits is more than 0, so adding units first would make the purchase fail
	// 
	// OR operator evaluates left to right so TryAddUnit only runs if the finish was set successfully
	// https://eel.is/c++draft/expr.log.or "Unlike |, || guarantees left-to-right evaluation; moreover, the second operand is not evaluated if the first operand evaluates to true."
	if (!PurchaseInfo.UnitsComponent->TrySetCurrentFinish(ProvidedFinish) || !PurchaseInfo.UnitsComponent->TryAddUnit(PurchaseInfo.UnitsToPurchase))
	{
		FlipperGameState->RefundFunds(PurchaseInfo.TotalCost, InteractorPlayerState);
	}
}

FFlipperInteractText AFlipperUnitProvider::GetInteractText(AActor* Interactor)
{
	FFlipperInteractText NewInteractText = FFlipperInteractText();

	FFlipperPurchaseInformation PurchaseInfo;
	const EFlipperPurchaseResult PurchaseResult = EvaluatePurchaseInformation(Interactor, PurchaseInfo);
	
	// don't show anything if the finish on the provider is invalid
	if (PurchaseResult == EFlipperPurchaseResult::InvalidFinish) return NewInteractText;
	
	NewInteractText.MainText = FText::Format(
		FText::FromString("Buy {0}"),
		PurchaseInfo.ProvidedSurfaceFinish->DisplayName
	);
	
	// get the tool name from the game instance
	UFlipperGameInstance* FlipperGameInstance = GetFlipperGameInstance();
	const FText* FoundToolName = FlipperGameInstance ? FlipperGameInstance->ToolNames.Find(UnitType) : nullptr;
	FText ToolDisplayName = FoundToolName ? FoundToolName->ToLower() : FText::FromString("tool"); // fallback to a generic name if gameinstance or foundtoolname is invalid
	
	switch (PurchaseResult)
	{
		case EFlipperPurchaseResult::WrongTool:
			NewInteractText.DescriptionText = FText::Format(FText::FromString("Equip {0} to buy"), ToolDisplayName);
			break;
		
		case EFlipperPurchaseResult::Full:
			NewInteractText.DescriptionText = FText::Format(FText::FromString("Your {0} is full"), ToolDisplayName);
			break;
	
		case EFlipperPurchaseResult::DifferentFinishHeld:
		{
			const UFlipperSurfaceFinishData* SurfaceFinishData = GetSurfaceData();
			const FFlipperSurfaceFinish* HeldSurfaceFinish = SurfaceFinishData ? SurfaceFinishData->GetFinish(PurchaseInfo.UnitsComponent->GetCurrentFinish()) : nullptr;
			if (HeldSurfaceFinish)
			{
				NewInteractText.DescriptionText = FText::Format(FText::FromString("You are holding {0}, sell it to buy this"), HeldSurfaceFinish->DisplayName);
			}
			else
			{
				NewInteractText.DescriptionText = FText::Format(FText::FromString("You aren't holding {0}, sell it to buy this"), PurchaseInfo.ProvidedSurfaceFinish->DisplayName);
			}
			break;
		}
		
		case EFlipperPurchaseResult::Success:
		{
			FCulturePtr USCulture = FInternationalization::Get().GetCulture(TEXT("en-US"));
			const FText FormattedTotalCost = FText::AsCurrencyBase(PurchaseInfo.TotalCost, TEXT("USD"), USCulture);
			const int32 UnitsToPurchase = PurchaseInfo.UnitsToPurchase;
			
			// team funds are available on every client as it's in game state, so warn if the team cannot afford the purchase
			const AFlipperGameState* FlipperGameState = GetFlipperGameState();
			if (FlipperGameState && PurchaseInfo.TotalCost > FlipperGameState->GetTeamFunds())
			{
				const FText FormattedTotalNeededMoney = FText::AsCurrencyBase(PurchaseInfo.TotalCost - FlipperGameState->GetTeamFunds(), TEXT("USD"), USCulture);
				NewInteractText.DescriptionText = FText::Format(FText::FromString("Team needs {0} more"), FormattedTotalNeededMoney);
				break;
			}
			
			// pluralisation in FText::Format - https://unreal-garden.com/tutorials/pluralizing-names/#fixing-our-localization-table
			NewInteractText.DescriptionText = FText::Format(
				FText::FromString("Buy {0} {0}|plural(one=unit,other=units) for {1}"),
				UnitsToPurchase,
				FormattedTotalCost
			);
			break;
		}
			
		default:
			break;
	}
	
	return NewInteractText;
}

