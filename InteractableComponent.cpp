// Copyright 2026 Becky Crawford — MIT License

#include "InteractableComponent.h"

UInteractableComponent::UInteractableComponent()
{
	// Interaction state is event-driven — no per-frame update needed.
	PrimaryComponentTick.bCanEverTick = false;
}

void UInteractableComponent::OnFocused_Implementation(AActor* Interactor)
{
	// Notify listeners that this actor has entered the interactor's focus.
	// Typical uses: enable outline/highlight effect, show interaction UI.
	OnFocusGained.Broadcast(Interactor);
}

void UInteractableComponent::OnUnfocused_Implementation(AActor* Interactor)
{
	// Notify listeners that this actor has left the interactor's focus.
	// Typical uses: disable outline/highlight effect, hide interaction UI.
	OnFocusLost.Broadcast(Interactor);
}

void UInteractableComponent::OnInteract_Implementation(AActor* Interactor)
{
	// Guard against interaction while ineligible before broadcasting.
	// CanInteract is called via Execute_ to correctly dispatch Blueprint overrides.
	if (IInteractable::Execute_CanInteract(this, Interactor))
	{
		OnInteracted.Broadcast(Interactor);
	}
}

bool UInteractableComponent::CanInteract_Implementation(AActor* Interactor) const
{
	// Reject if interaction is disabled or no valid interactor was provided.
	if (!bIsInteractable || !Interactor) return false;

	// If tag requirements are set, the interactor must carry all of them.
	if (RequiredInteractorTags.Num() > 0)
	{
		for (const FName& Tag : RequiredInteractorTags)
		{
			if (!Interactor->ActorHasTag(Tag)) return false;
		}
	}

	return true;
}

void UInteractableComponent::OnAction_Implementation(AActor* Interactor)
{
	// Guard against action while ineligible before broadcasting.
	// CanAction is called via Execute_ to correctly dispatch Blueprint overrides.
	if (IInteractable::Execute_CanAction(this, Interactor))
	{
		OnActioned.Broadcast(Interactor);
	}
}

bool UInteractableComponent::CanAction_Implementation(AActor* Interactor) const
{
	// Reject if action is disabled or no valid interactor was provided.
	if (!bIsActionable || !Interactor) return false;

	// If tag requirements are set, the interactor must carry all of them.
	if (RequiredInteractorTags.Num() > 0)
	{
		for (const FName& Tag : RequiredInteractorTags)
		{
			if (!Interactor->ActorHasTag(Tag)) return false;
		}
	}

	return true;
}