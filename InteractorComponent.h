// Copyright 2026 Becky Crawford — MIT License

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Camera/CameraComponent.h"
#include "InteractorComponent.generated.h"

class UInteractableComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFocusedInteractableChanged, UInteractableComponent*, NewInteractable);

/**
 * UInteractorComponent
 *
 * Add this component to the player character to enable interaction with any actor
 * that carries a UInteractableComponent. Each tick, this component performs a sphere
 * sweep from the player's camera along their look direction. The first interactable
 * actor hit becomes the focused interactable.
 *
 * Interaction range is measured from the character's world location rather than the
 * camera, ensuring consistent reach regardless of spring arm / camera zoom level.
 *
 * Call TryInteract() and TryAction() from input bindings to dispatch the corresponding
 * events to the currently focused interactable.
 *
 * The OnFocusedInteractableChanged delegate fires whenever focus changes (including
 * when it clears to null), and can be used to drive interaction UI.
 */
UCLASS( ClassGroup=(Interaction), meta=(BlueprintSpawnableComponent) )
class MYSURVIVALPROJECT_API UInteractorComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UInteractorComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
		FActorComponentTickFunction* ThisTickFunction) override;

	// Attempts to trigger the primary interaction on the currently focused interactable. Bind to interact input.
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void TryInteract();

	// Attempts to trigger the secondary interaction on the currently focused interactable. Bind to action input.
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void TryAction();

	// Returns the interactable currently in focus, or null if none. Used for UI or validation logic
	UFUNCTION(BlueprintPure, Category = "Interaction")
	UInteractableComponent* GetFocusedInteractable() const { return FocusedInteractable; }

	// Fires whenever the focused interactable changes, including when it clears to null. Used to update interaction UI.
	UPROPERTY(BlueprintAssignable, Category = "Interaction")
	FOnFocusedInteractableChanged OnFocusedInteractableChanged;

	// Maximum reach of the interaction trace, measured from the character's world location.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	float InteractionRange = 250.f;

	// Radius of the sphere sweep. A larger value gives more forgiving hit detection at the cost of precision.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	float TraceRadius = 30.f; 

	/**
	 * Component tag used to identify which CameraComponent to trace from.
	 * If no camera carries this tag, falls back to the first camera found on the owner.
	 * Compatible with spring arm setups — the trace originates from the camera's world position.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	FName CameraComponentTag = "MainCamera";

	// Stores the most recent hit result from the interaction trace. Exposed for use in debug widgets.
	UPROPERTY(BlueprintReadOnly, Category = "Interaction|Debug")
	FHitResult DebugLastHitResult;

	// True if the most recent interaction trace produced a hit. False otherwise.
	UPROPERTY(BlueprintReadOnly, Category = "Interaction|Debug")
	bool bDebugLastTraceHit = false;

	// Whether or not to draw the trace debug line visualization
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction|Debug")
	bool bDebugLineTrace = false;

	// Whether or not to draw the trace debug sphere visualization
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction|Debug")
	bool bDebugSphereTrace = false;

	// Whether or not to draw the trace debug impact point visualization
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction|Debug")
	bool bDebugImpactPoint = false;

	// Whether or not to draw the trace debug impact normal visualization
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction|Debug")
	bool bDebugImpactNormal = false;

protected:
	virtual void BeginPlay() override;

private:	
	// Performs the sphere sweep each tick and updates the focused interactable accordingly.
	void PerformInteractionTrace();

	/**
	 * Updates the focused interactable, broadcasting focus gained/lost events as needed.
	 * Does nothing if the new interactable is the same as the current one.
	*/
	void SetFocusedInteractable(UInteractableComponent* NewInteractable);

	// The interactable currently in the player's focus. Null if none.
	UPROPERTY()
	UInteractableComponent* FocusedInteractable = nullptr;

	// Cached reference to the owner's camera component, resolved once in BeginPlay.
	UPROPERTY()
	UCameraComponent* CachedCamera = nullptr;

	// The length of the arrow showing the debug impact normal 
	UPROPERTY()
	float DebugImpactNormalLength = 20.f;

	// The screen-space size of the debug impact point
	UPROPERTY()
	float DebugImpactPointSize = 12.f;
};
