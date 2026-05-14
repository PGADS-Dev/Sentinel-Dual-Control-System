# Sentinel v0.1 Fault Matrix

## Purpose

This document maps each mandatory fault scenario to a detection signal, expected system response, observable trace, and demo method.

The goal is to avoid vague claims. Each fault must be explicit, reproducible, and reviewable.

## Fault matrix

| ID | Fault scenario | Detection signal | Expected state response | Observable trace | Demo method |
|---|---|---|---|---|---|
| F001 | Loss of heartbeat from one worker | Peer heartbeat not received before timeout | `NOMINAL` -> `DEGRADED`, then `FAIL_SAFE` if the fault escalates | `SENTINEL_FAULT_HEARTBEAT_LOST`, peer node ID, timeout duration, state transition | Stop heartbeat emission from one worker or block its heartbeat messages |
| F002 | Incoherent peer state | Peer state conflicts with local expectation | `NOMINAL` -> `DEGRADED` | `SENTINEL_FAULT_INCOHERENT_PEER_STATE`, local state, peer state | Force one worker to report an invalid or conflicting state |
| F003 | Communication loss on CAN or equivalent exchange path | No valid exchange on CAN within the configured window | `INIT`, `NOMINAL`, or `DEGRADED` -> `FAIL_SAFE` | `SENTINEL_FAULT_COMMUNICATION_LOST`, bus status, last valid message timestamp | Disconnect or disable CAN exchange in a controlled way |

## Expected fault handling principles

For every fault:

- detection must be explicit
- response must follow documented state transition rules
- entry into `DEGRADED` or `FAIL_SAFE` must be visible
- the supervisor must be able to reconstruct the order of events
- the system must not silently return to `NOMINAL`

## Fault severity guidance

### Heartbeat loss

Heartbeat loss starts as a peer freshness problem.

A short heartbeat warning may enter `DEGRADED` if the system still has enough information to remain controlled.

A prolonged heartbeat loss must escalate to `FAIL_SAFE`.

### Incoherent peer state

Incoherent peer state means the two workers disagree about the system state.

This is abnormal because the system can no longer assume both workers share the same view of reality.

The minimum response is `DEGRADED`.

The response may escalate to `FAIL_SAFE` if the disagreement affects safe behavior.

### Communication loss

Communication loss means the exchange path itself is no longer reliable.

This is more severe than a single delayed heartbeat because it affects system supervision.

The expected response is `FAIL_SAFE` unless a later document defines a stricter degraded policy.

## Required observable fields

Each fault trace should expose, when available:

- timestamp
- fault ID
- worker ID
- previous state
- next state
- detection reason
- last valid peer message time
- current heartbeat counter or sequence number
- supervisor observation time

## Done criteria

This fault matrix is acceptable when:

- all mandatory fault scenarios are listed
- each fault has a detection signal
- each fault has an expected state response
- each fault has an observable trace
- each fault has a simple demo method
