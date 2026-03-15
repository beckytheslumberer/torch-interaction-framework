// Copyright 2026 Becky Crawford — MIT License

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Interactable.generated.h"

/**
 * UInteractable
 *
 * Required boilerplate class for Unreal's interface reflection system.
 * Do not add functionality here — use IInteractable below.
 */
UINTERFACE(MinimalAPI)
class UInteractable : public UInterface
{
	GENERATED_BODY()
};

/**
 * IInteractable
 *
 * Interface that defines the contract for any actor that can be interacted with
 * or actioned by the InteractorComponent. Implementing this interface on an actor
 * or component allows it to participate in the interaction system without requiring
 * any specific base class.
 *
 * All functions are BlueprintNativeEvent, meaning they can be overridden in both
 * C++ (via _Implementation) and Blueprint. Always call these via Execute_ wrappers
 * (e.g. IInteractable::Execute_OnInteract) rather than directly.
 *
 * Two interaction types are supported:
 *   - Interact: Primary interaction Input E (e.g. picking up, climbing)
 *   - Action:   Secondary interaction Input LMB (e.g. chopping, digging, lighting)
 */
class MYSURVIVALPROJECT_API IInteractable
{
	GENERATED_BODY()

public:
	// Called by the InteractorComponent when this actor enters the interaction trace. (focus gained)
	UFUNCTION(BlueprintNativeEvent, Category = "Interaction")
	void OnFocused(AActor* Interactor);

	// Called by the InteractorComponent when this actor leaves the interaction trace. (focus lost)
	UFUNCTION(BlueprintNativeEvent, Category = "Interaction")
	void OnUnfocused(AActor* Interactor);

	// Called when the InteractorComponent triggers the primary interact input while this actor is focused.
	UFUNCTION(BlueprintNativeEvent, Category = "Interaction")
	void OnInteract(AActor* Interactor);

	// Returns whether this actor is currently eligible for primary interaction.
	UFUNCTION(BlueprintNativeEvent, Category = "Interaction")
	bool CanInteract(AActor* Interactor) const;

	// Called when the InteractorComponent triggers the secondary interact input while this actor is focused.
	UFUNCTION(BlueprintNativeEvent, Category = "Interaction")
	void OnAction(AActor* Interactor);

	// Returns whether this actor is currently eligible for the secondary action.
	UFUNCTION(BlueprintNativeEvent, Category = "Interaction")
	bool CanAction(AActor* Interactor) const;
};
