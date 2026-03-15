// Copyright 2026 Becky Crawford — MIT License

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Interactable.h"
#include "InteractableComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInteractSignature, AActor*, Interactor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFocusSignature, AActor*, Interactor);


/**
 * UInteractableComponent
 *
 * Add this component to any actor that should be interactable in the world.
 * It implements the IInteractable interface and exposes Blueprint-assignable delegates
 * for each interaction event, allowing per-actor behaviour to be defined entirely
 * in Blueprint without subclassing.
 *
 * Supports two interaction types:
 *   - Interact: Primary interaction driven by OnInteracted delegate.
 *   - Action:   Secondary interaction driven by OnActioned delegate.
 *
 * Both interaction types can be independently enabled or disabled at runtime via
 * bIsInteractable and bIsActionable. Access restriction is also supported via
 * RequiredInteractorTags, which limits interactions to actors carrying specific tags.
 *
 * Usage:
 *   1. Add this component to an actor Blueprint.
 *   2. Bind the OnInteracted and/or OnActioned delegates to interaction logic.
 */
UCLASS( ClassGroup=(Interaction), meta=(BlueprintSpawnableComponent) )
class MYSURVIVALPROJECT_API UInteractableComponent : public UActorComponent, public IInteractable
{
	GENERATED_BODY()

public:	
	UInteractableComponent();

	// --- Interactable Implementation ---
	virtual void OnFocused_Implementation(AActor* Interactor) override;
	virtual void OnUnfocused_Implementation(AActor* Interactor) override;
	virtual void OnInteract_Implementation(AActor* Interactor) override;
	virtual bool CanInteract_Implementation(AActor* Interactor) const override;
	virtual void OnAction_Implementation(AActor* Interactor) override;
	virtual bool CanAction_Implementation(AActor* Interactor) const override;

	// --- Delegates (bind in Blueprint or C++) ---
	// Broadcast when the primary interact input is triggered and CanInteract passes. Bind interaction logic here.
	UPROPERTY(BlueprintAssignable, Category = "Interaction")
	FOnInteractSignature OnInteracted;

	// Broadcast when the secondary action input is triggered and CanAction passes. Bind action logic here.
	UPROPERTY(BlueprintAssignable, Category = "Interaction")
	FOnInteractSignature OnActioned;
	// Broadcast when the InteractorComponent's trace begins hitting this actor. Use to drive highlight effects or UI.
	UPROPERTY(BlueprintAssignable, Category = "Interaction")
	FOnFocusSignature OnFocusGained;

	// Broadcast when the InteractorComponent's trace stops hitting this actor. Use to clear highlight effects or UI.
	UPROPERTY(BlueprintAssignable, Category = "Interaction")
	FOnFocusSignature OnFocusLost;

	// --- Config ---
	// Whether this actor currently accepts primary interact input. Can be toggled at runtime.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	bool bIsInteractable = true;

	// Whether this actor currently accepts secondary action input. Disabled by default — enable on actors that need it
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	bool bIsActionable = false;

	/**
	 * Optional tag filter for restricting who can interact with this actor.
	 * If populated, the interacting actor must have ALL listed tags for CanInteract
	 * and CanAction to return true. Leave empty to allow any interactor.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	TArray<FName> RequiredInteractorTags;
		
};
