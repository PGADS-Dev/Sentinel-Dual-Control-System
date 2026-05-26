# Supervisor Simulation Placeholder

## Purpose

This directory is reserved for the future Sentinel v0.1 supervisor simulation.

The supervisor is intended to represent the Raspberry Pi side of the system. Its role is to observe worker behavior, collect visible traces, and make fault-handling decisions understandable from the outside.

This is intentionally only a placeholder for now.

## Role of the supervisor

The supervisor is an observer first.

It is expected to:

- receive worker state information
- observe state transitions
- observe detected faults
- expose timestamped logs
- help reconstruct what happened during a fault scenario
- support the future demo sequence

The supervisor is not responsible for making the workers safe.

Worker safety decisions must remain local and deterministic inside the worker state machine.

## Expected logs

Future supervisor logs should make the following information visible:

- timestamp
- worker identifier
- current worker state
- received event or detected fault
- previous state
- next state
- fault name when applicable

Example future log shape:

```text
[001245 ms] worker=A event=HEARTBEAT_WARNING state=NOMINAL -> DEGRADED
[001980 ms] worker=B fault=COMMUNICATION_LOST state=DEGRADED -> FAIL_SAFE
```

## Future demo purpose

The future supervisor simulation will help demonstrate that Sentinel v0.1 can:

- show normal operation
- show degraded-mode entry
- show fail-safe entry
- show fault detection
- show state transitions clearly
- provide trace evidence for review

The goal is not to build a dashboard.

The goal is to produce simple, readable, timestamped evidence of system behavior under fault.

## v0.1 boundary

For Sentinel v0.1, this directory does not include:

- a complete supervisor implementation
- a dashboard
- a web interface
- a database
- a systemd service
- a full Raspberry Pi deployment
- advanced trace tooling
- cloud connectivity

These may be considered later if they strengthen the proof, but they are not required for the v0.1 skeleton.

## Current status

Current status: placeholder only.

The worker state machine is the authoritative source of safety behavior for this stage.

The supervisor will later expose state transitions, fault detection, and trace capture in a simple observable form.
