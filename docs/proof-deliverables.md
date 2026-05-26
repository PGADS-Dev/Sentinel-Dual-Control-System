# Sentinel v0.1 Proof Deliverables

## Purpose

Sentinel v0.1 is valuable only if it produces reusable proof material.

The goal is not to build many features. The goal is to produce clear public evidence that the system detects faults, decides deterministically, transitions intentionally, degrades deliberately, fails safely, and exposes what happened.

## Required deliverables

| Deliverable | Purpose | Status |
|---|---|---|
| Architecture document | Explain the system boundary and node responsibilities | Planned |
| Architecture diagram | Show workers, CAN bus, supervisor, logs, and fault injection | Planned |
| State machine document | Define operating states and transition rules | Planned |
| State machine diagram | Show deterministic behavior visually | Planned |
| Fault matrix | Map each fault to detection, response, trace, and demo method | Planned |
| Observability requirements | Define what must be visible from outside the system | Planned |
| Technical write-up | Explain what the proof demonstrates and what it does not | Planned |
| Demo trace or capture | Provide execution evidence for at least one fault scenario | Planned |

## Minimum proof package

The minimum acceptable v0.1 proof package is:

1. `docs/v0.1-scope.md`
2. `docs/architecture.md`
3. `docs/state-machine.md`
4. `docs/fault-matrix.md`
5. `docs/observability.md`
6. One short trace or example log showing a fault and state transition

## Public proof material

The public material should be understandable without access to private client systems.

Recommended public outputs:

- short README summary
- architecture diagram
- state machine diagram
- sanitized fault matrix
- example supervisor logs
- short technical write-up
- optional demo video or GIF

## Commercial relevance

Sentinel v0.1 supports the Embedded Failure Triage Sprint by proving the mindset behind the offer:

- understand failure symptoms
- separate facts from assumptions
- identify credible fault hypotheses
- expose missing evidence
- reason about degraded and fail-safe behavior
- produce a structured next-step plan

## Review criteria

A proof deliverable is useful only if it is:

- simple
- technically honest
- directly tied to the fault-handling proof
- easy to explain to an engineering manager
- credible to a technical reviewer
- not inflated into a product claim

## Not accepted

The following are not useful v0.1 proof deliverables:

- vague diagrams with no fault behavior
- screenshots of a dashboard without technical depth
- large feature lists
- claims without logs or traces
- complex architecture that cannot be demonstrated
- anything that distracts from fault detection and deterministic response

## Done criteria

The v0.1 proof package is acceptable when it can answer:

- what the system is
- what fault scenarios it covers
- how faults are detected
- how decisions are made
- how states change
- what becomes observable
- what is intentionally not claimed
