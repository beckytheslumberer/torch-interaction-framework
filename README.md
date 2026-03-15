# Torch Interaction Framework
### A scalable component-based interaction system for Unreal Engine 5

[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](LICENSE)
[![Unreal Engine](https://img.shields.io/badge/Unreal%20Engine-5-black?logo=unrealengine)](https://www.unrealengine.com/)
[![Language](https://img.shields.io/badge/Language-C%2B%2B%20%2F%20Blueprint-orange)]()

---

> **📖 Deep dive →** [Becky Crawford Portfolio - Interaction Framework](https://beckytheslumberer.github.io/becky-crawford-portfolio/#/torch-interaction)

---

## Overview

Built for a survival game in UE5, this framework solves the problem of scaling interaction logic across a growing number of actors — items, tools, trees, dig spots, ladders, bridges, and more — without coupling any of that logic to the player character.

The system is built around three C++ files: an interface that defines the interaction contract, a component that any actor can adopt to become interactable, and a component that sits on the player and handles tracing and input dispatch. Once compiled, the entire system is usable from Blueprint with no further C++ required.

**Adding a new interactable is a two-step process:** drop `InteractableComponent` onto an actor, bind the `OnInteracted` delegate, and it works automatically.

---

## Features

- **No coupling** — the player character holds no references to any specific interactable type
- **Two interaction types** — primary Interact and secondary Action, independently togglable per actor
- **Blueprint-first** — all interaction logic is wired via assignable delegates; no subclassing needed
- **Consistent reach** — interaction range is anchored to the character's world position, so spring arm zoom never affects reach
- **Tag-based access control** — actors can require the interactor to carry specific tags
- **Focus events** — `OnFocusGained` / `OnFocusLost` delegates for driving highlight effects and UI
- **Debug tooling** — live trace visualisation and a `DebugLastHitResult` property for runtime debug widgets

---

## Files

```
Source/
└── YourProject/
    ├── Interactable.h             # IInteractable interface
    ├── Interactable.cpp
    ├── InteractableComponent.h    # Add to any interactable world actor
    ├── InteractableComponent.cpp
    ├── InteractorComponent.h      # Add to the player character
    └── InteractorComponent.cpp
```

---

## Quick Start

### 1. Add to your project

Copy the six files into your project's `Source/YourProject/` directory. Replace `MYSURVIVALPROJECT_API` with your own project's API macro.

### 2. Add `InteractorComponent` to your Character Blueprint

In BeginPlay, bind `OnFocusedInteractableChanged` to update your interaction UI. Then wire your input actions:

```
IA_Interact (Triggered) → InteractorComponent → TryInteract
IA_Action   (Triggered) → InteractorComponent → TryAction
```

### 3. Make an actor interactable

Add `InteractableComponent` to any Blueprint actor. Then bind the `OnInteracted` delegate to your logic:

```
OnInteracted (Interactor)
    → [Your pickup / use logic here]
    → InteractableComponent → Set Is Interactable (false)   ← optional, prevents re-triggering
```

### 4. Set up collision

Create a custom trace channel named `Interaction` in **Project Settings → Collision**. Set its default response to `Ignore`, then set interactable actors to `Block` it. This prevents the trace from hitting unintended geometry.

---

## Interaction Types

| Property | Default | Purpose |
|---|---|---|
| `bIsInteractable` | `true` | Enables the primary interact input |
| `bIsActionable` | `false` | Enables the secondary action input |
| `RequiredInteractorTags` | *(empty)* | Restricts who can interact — leave empty for no restriction |

---

## Notes

- All interface functions are `BlueprintNativeEvent` — always call them via `IInteractable::Execute_*` wrappers, never directly.
- The sphere sweep uses `ECC_GameTraceChannel2` by default — update this constant in `InteractorComponent.cpp` to match the channel number assigned to your `Interaction` channel in Project Settings.
- The debug line draw and `UE_LOG` hit output in `PerformInteractionTrace` should be removed or wrapped in a `#if !UE_BUILD_SHIPPING` guard before shipping.

---

## License

MIT — see [LICENSE](LICENSE).
