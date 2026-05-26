# Sentinel v0.1 Observability Requirements

## Purpose

Sentinel v0.1 must make internal system behavior visible from the outside.

It is not enough for the system to react correctly. The reaction must be understandable, reviewable, and reproducible from logs or traces.

## Observability goal

The supervisor must help answer these questions:

- What happened?
- When did it happen?
- Which node observed it?
- What fault was detected?
- What state was the system in before the fault?
- What state did the system enter after the fault?
- What evidence is missing, if any?

## Minimum observable events

Sentinel v0.1 must expose at least:

- worker startup
- heartbeat received
- heartbeat missed
- peer status received
- state transition
- fault detected
- entry into `DEGRADED`
- entry into `FAIL_SAFE`
- manual reset or recovery action

## Required log fields

Each supervisor-side event should include, when available:

- timestamp
- event type
- worker ID
- previous state
- next state
- fault ID
- detection reason
- heartbeat counter or sequence
- last valid peer message timestamp
- raw or summarized CAN frame reference

## Example event format

```json
{
  "timestamp_ms": 123456,
  "event_type": "STATE_TRANSITION",
  "worker_id": "worker_a",
  "previous_state": "NOMINAL",
  "next_state": "DEGRADED",
  "fault_id": "F001",
  "reason": "peer heartbeat timeout",
  "last_peer_message_age_ms": 520
}
```

## Supervisor responsibilities

The supervisor must:

- timestamp observed events
- preserve event order
- expose state transitions clearly
- expose fault detection clearly
- make degraded and fail-safe entry obvious
- avoid hiding missing evidence

The supervisor does not need a polished UI.

Text logs, structured JSON logs, or trace captures are enough for v0.1.

## Worker-side observability

Each worker should expose enough information for the supervisor to build a useful timeline:

- current operating state
- heartbeat counter
- health status
- fault flags
- state transition notifications

Worker observability should remain simple and bounded.

## Evidence quality rules

Good evidence is:

- timestamped
- ordered
- tied to a node
- tied to a fault or state transition
- easy to compare with expected behavior

Weak evidence is:

- missing timestamps
- ambiguous about which node emitted it
- unable to explain why a transition happened
- too noisy to reconstruct the event sequence

## Out of scope

Sentinel v0.1 does not require:

- advanced dashboard
- web visualization
- mobile visualization
- database-backed event history
- remote telemetry
- cloud logging
- pretty charts

The observability layer does not need to be beautiful. It needs to be clear.

## Done criteria

Observability is acceptable when a reviewer can read the logs or traces and answer:

- which fault was injected
- which worker observed the problem
- when the fault was detected
- which state transition occurred
- whether the system entered `DEGRADED` or `FAIL_SAFE`
- whether the behavior matched the fault matrix
