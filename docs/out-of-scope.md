# Sentinel v0.1 Out of Scope

## Purpose

This document protects Sentinel v0.1 from scope creep.

Sentinel v0.1 must remain a narrow technical proof of fault-aware embedded behavior. Anything that does not directly strengthen the central proof should stay out.

## Core rule

A feature is allowed in v0.1 only if it directly improves the proof that the system can:

- detect abnormal conditions
- decide deterministically
- transition through explicit states
- enter degraded behavior intentionally
- enter fail-safe behavior deliberately
- expose that behavior through observable traces

If it does not strengthen that proof, it is out of scope.

## Explicitly out of scope

The following items are not part of Sentinel v0.1:

- polished dashboard
- advanced UI
- rich HMI
- web interface
- mobile application
- cloud connectivity
- remote fleet management
- OTA update pipeline
- rollback system
- Bluetooth features
- Wi-Fi features
- persistent database
- persistent history feature
- advanced Linux crash capture pipeline
- large-scale distributed coordination
- multi-node scaling beyond two workers and one supervisor
- non-essential sensors
- complex tooling
- full industrial product architecture
- certification or compliance material
- exhaustive fault coverage

## Postponed to later versions

These topics may become useful later, but they are not required for v0.1:

- richer supervisor UI
- advanced HMI
- full Linux crash capture pipeline
- remote telemetry
- persistent event storage
- expanded fault library
- realistic actuator or sensor simulation
- refined voting logic
- update and rollback reliability work
- remote diagnostics features

## Why this boundary exists

A narrow v0.1 is stronger than a broad unfinished prototype.

If the scope becomes too wide:

1. the proof message becomes unclear
2. the system becomes harder to finish
3. the result looks like an unfinished product instead of a serious engineering asset

Sentinel v0.1 should demonstrate judgment: select the core problem, isolate it, and prove it cleanly.

## Decision checklist

Before adding anything to v0.1, ask:

- Does this improve fault detection?
- Does this improve deterministic response?
- Does this clarify the state machine?
- Does this improve degraded or fail-safe behavior?
- Does this improve observability?
- Does this make the proof easier to review?

If the answer is no, do not add it.

## Done criteria

This boundary is respected when:

- v0.1 remains focused on the mandatory fault scenarios
- no UI work delays the proof
- no cloud or dashboard work is added
- no advanced tooling is added before the core behavior is proven
- the public message stays simple and credible
