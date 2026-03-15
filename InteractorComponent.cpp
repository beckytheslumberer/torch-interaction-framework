// Copyright 2026 Becky Crawford — MIT License

#include "InteractorComponent.h"
#include "InteractableComponent.h"
#include "DrawDebugHelpers.h"
#include "Engine/EngineTypes.h"

UInteractorComponent::UInteractorComponent()
{
	// Interaction tracing must run every frame to keep focus state current.
	PrimaryComponentTick.bCanEverTick = true;
}

void UInteractorComponent::BeginPlay()
{
	Super::BeginPlay();
	
	// Resolve and cache the camera component once to avoid repeated searches each tick.
	// Prefers a camera matching CameraComponentTag; falls back to the first camera found.
	if (AActor* Owner = GetOwner())
	{
		TArray<UCameraComponent*> Cameras;
		Owner->GetComponents<UCameraComponent>(Cameras);
		for (UCameraComponent* Cam : Cameras)
		{
			if (Cam->ComponentHasTag(CameraComponentTag))
			{
				CachedCamera = Cam;
				break;
			}
		}
		// Fallback: use the first available camera if none matched the tag.
		if (!CachedCamera && Cameras.Num() > 0)
		{
			CachedCamera = Cameras[0];
		}
	}
}


void UInteractorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	PerformInteractionTrace();
}

void UInteractorComponent::PerformInteractionTrace()
{
	AActor* Owner = GetOwner();
	if (!Owner) return;

	FVector TraceStart;
	FVector TraceDirection;

	if (CachedCamera)
	{
		TraceDirection = CachedCamera->GetForwardVector();

		// Offset the trace start forward to avoid immediately hitting geometry behind the camera.
		TraceStart = CachedCamera->GetComponentLocation() + TraceDirection * 30.f;
	}
	else
	{
		// Fallback for characters without a camera component.
		FRotator EyeRot;
		Owner->GetActorEyesViewPoint(TraceStart, EyeRot);
		TraceDirection = EyeRot.Vector();
	}
	
	// Anchor the trace end to the character's world location rather than the camera.
	// This keeps interaction range consistent regardless of spring arm / zoom level.
	const FVector CharacterLocation = Owner->GetActorLocation();
	const FVector TraceEnd = CharacterLocation + (TraceDirection * InteractionRange);

	FHitResult HitResult;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(Owner);

	// Ignore all primitive components on the owner (capsule, mesh, etc.) to prevent
	// the trace from terminating on the player's own collision.
	TArray<UPrimitiveComponent*> OwnerPrimitives;
	Owner->GetComponents<UPrimitiveComponent>(OwnerPrimitives);
	for (UPrimitiveComponent* Primitive : OwnerPrimitives)
	{
		Params.AddIgnoredComponent(Primitive);
	}

	// Use the custom Interaction trace channel so only actors set to Block this
	// channel are candidates — avoids unintended hits on environmental geometry.
	ECollisionChannel InteractChannel = ECC_GameTraceChannel2;

	const bool bHit = GetWorld()->SweepSingleByChannel(
		HitResult,
		TraceStart,
		TraceEnd,
		FQuat::Identity,
		InteractChannel,
		FCollisionShape::MakeSphere(TraceRadius),
		Params
	);

	// Store the hit result for debug access
	DebugLastHitResult = HitResult;
	bDebugLastTraceHit = bHit;

	// Override the hit distance to be character-relative rather than camera-relative
	// so the debug widget reflects meaningful in-world reach values.
	if (bHit)
	{
		DebugLastHitResult.Distance = FVector::Dist(Owner->GetActorLocation(), HitResult.ImpactPoint);
	}

	// Attempt to resolve an interactable from the hit actor.
	UInteractableComponent* HitInteractable = nullptr;

	if (bHit && HitResult.GetActor())
	{
		HitInteractable = HitResult.GetActor()->FindComponentByClass<UInteractableComponent>();

		// Discard the hit if the interactable exists but is not currently eligible.
		if (HitInteractable && !IInteractable::Execute_CanInteract(HitInteractable, Owner))
		{
			HitInteractable = nullptr;
		}
	}

	// Log hit component details to the output log for debugging.
	if (bHit && HitResult.GetComponent())
	{
		UE_LOG(LogTemp, Warning, TEXT("Trace hit: %s on %s"),
			*HitResult.GetComponent()->GetName(),
			*HitResult.GetActor()->GetName());
	}

	SetFocusedInteractable(HitInteractable);

	// Visual debug line — green on hit, red on miss.
	// DrawDebugLine(GetWorld(), TraceStart, TraceEnd, bHit ? FColor::Green : FColor::Red, false, -1.f, 0, 2.f);
}

void UInteractorComponent::TryInteract()
{
	// Dispatch the primary interact event to the focused interactable.
	// CanInteract is checked internally by the interactable before broadcasting.
	if (FocusedInteractable)
	{
		IInteractable::Execute_OnInteract(FocusedInteractable, GetOwner());
	}
}

void UInteractorComponent::SetFocusedInteractable(UInteractableComponent* NewInteractable)
{
	// Disregard if focus hasn't changed — avoids redundant delegate broadcasts.
	if (NewInteractable == FocusedInteractable) return;

	// Notify the previous interactable that it has lost focus.
	if (FocusedInteractable)
	{
		IInteractable::Execute_OnUnfocused(FocusedInteractable, GetOwner());
	}

	FocusedInteractable = NewInteractable;

	// Notify the new interactable that it has gained focus.
	if (FocusedInteractable)
	{
		IInteractable::Execute_OnFocused(FocusedInteractable, GetOwner());
	}

	// Notify external listeners that the focused interactable has changed.
	OnFocusedInteractableChanged.Broadcast(FocusedInteractable);
}

void UInteractorComponent::TryAction()
{
	// Dispatch the secondary action event to the focused interactable.
	// CanAction is checked internally by the interactable before broadcasting.
	if (FocusedInteractable)
	{
		IInteractable::Execute_OnAction(FocusedInteractable, GetOwner());
	}
}