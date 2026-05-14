# CrashLab Scenario Roadmap

## Purpose

CrashLab is a set of controlled failure scenarios used to produce public Sentinel reliability proof assets.

Each scenario must be:
- reproducible
- observable
- technically realistic
- suitable for a public crash autopsy
- linked to a clear reliability lesson

## Scenario Selection Criteria

A scenario is selected only if it demonstrates at least one of the following:
- fault detection
- deterministic state transition
- degraded mode behavior
- fail-safe behavior
- missing or insufficient observability
- root cause analysis from logs/traces
- recovery behavior after failure

## Scenario Template

Each scenario should define:

- failure description
- system context
- injection method
- expected behavior
- observed evidence
- state transition
- mitigation
- verification method
- public autopsy link

## Candidate Scenarios

| Priority | Scenario | Failure Type | What It Proves | Public Output |
|----------|----------|--------------|----------------|---------------|
| P1 | Watchdog reset caused by blocked worker loop | Runtime freeze / watchdog | The system can detect blocked execution and explain the reset cause | Crash autopsy |
| P1 | Heartbeat loss between controllers | Supervision fault | The system can detect peer silence and transition intentionally | Crash autopsy |
| P1 | CAN communication timeout | Communication lost | The system reacts deterministically to loss of exchange | Crash autopsy |
| P1 | Incoherent state between Worker A and Worker B | State disagreement | Cross-monitoring detects unsafe inconsistency | Crash autopsy |
| P2 | Unsafe recovery after restart | Recovery logic fault | Recovery must be explicit, bounded, and observable | Crash autopsy |
| P2 | Watchdog reset without useful logs | Observability failure | Missing evidence makes root cause analysis weaker | Methodology note |
| P2 | Loop timing overrun under load | Timing fault | Runtime timing must be measured and bounded | Crash autopsy |
| P2 | Stale sensor/status value accepted as valid | Data freshness fault | Old data must not be treated as current truth | Crash autopsy |
| P3 | Fault flag cleared too early | Fault handling flaw | Failures should not disappear before being reviewed | Crash autopsy |
| P3 | Degraded mode entered without operator-visible trace | Observability / HMI fault | Degraded behavior must be externally visible | Methodology note |

## First Scenario To Implement

The first scenario should be:

**Watchdog reset caused by blocked worker loop**

Reason:
- simple to simulate
- easy to explain publicly
- strongly linked to embedded reliability
- produces a clear failure timeline
- naturally fits the public crash autopsy template

## Done Criteria

This roadmap is considered complete when:
- 10 candidate failure scenarios are listed
- each scenario has a clear proof objective
- the first scenario is selected
- the roadmap links naturally to the public crash autopsy template
