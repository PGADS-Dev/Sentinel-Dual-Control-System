# Public Crash Autopsy Template

## 1. Incident Summary
Short explanation of what happened.

## 2. System Context
What system/component was involved?
What was the expected behavior?

## 3. Observed Behavior
What actually happened?
What symptoms were visible?

## 4. Impact
What was affected?
Was it a crash, freeze, reset, degraded mode, data loss, communication loss?

## 5. Timeline
- T0: system running normally
- T1: abnormal condition appears
- T2: detection
- T3: transition to degraded/fail-safe
- T4: recovery or shutdown

## 6. Evidence Collected
- logs
- traces
- core dump
- serial output
- watchdog event
- state transition trace
- reproduction notes

## 7. Root Cause Analysis
### Direct cause
What directly triggered the failure?

### Contributing factors
What made the failure possible or harder to detect?

### Why it was not caught earlier
What was missing in tests, logs, monitoring, validation, or design?

## 8. Mitigation
### Immediate containment
What can be done quickly to reduce risk?

### Short-term fix
What should be fixed soon?

### Long-term prevention
What should be changed in architecture, tests, instrumentation, watchdogs, or state handling?

## 9. Verification
How do we prove the fix or mitigation works?

## 10. Lessons Learned
What engineering rule should we remember from this?

## 11. Public Notes
What information was removed or anonymized for public release?
