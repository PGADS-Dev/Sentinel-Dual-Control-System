/*
 * SNT-TEST-001 — Sentinel Fault Validation Suite
 *
 * =============================================================================
 * EVENT MAPPING (specification → implementation)
 * =============================================================================
 *
 *   SYSTEM_START        → SENTINEL_EVENT_STARTUP_COMPLETE
 *   HEARTBEAT_TIMEOUT   → SENTINEL_EVENT_HEARTBEAT_WARNING
 *   VALUE_INCONSISTENT  → SENTINEL_EVENT_INCOHERENT_PEER_STATE
 *   COMM_LOST           → SENTINEL_EVENT_COMMUNICATION_LOST
 *   RESET_REQUESTED     → SENTINEL_EVENT_MANUAL_RESET
 *
 * =============================================================================
 * DIVERGENCES IDENTIFIED BETWEEN THE SPECIFICATION AND THE CURRENT ENGINE
 * =============================================================================
 *
 * DIVERGENCE D1 — NOMINAL + COMM_LOST must transition to DEGRADED, not FAIL_SAFE
 *   Specification: NOMINAL + COMM_LOST → DEGRADED
 *   Current engine:   NOMINAL + COMM_LOST → FAIL_SAFE (sentinel_state.c, linha 67–68)
 *   Impact:       Tests T04 and T06 will fail.
 *
 * DIVERGENCE D2 — DEGRADED does not escalate to FAIL_SAFE after a repeated heartbeat failure
 *   Specification: DEGRADED + HEARTBEAT_TIMEOUT (new failure) → FAIL_SAFE
 *   Current engine:   DEGRADED + HEARTBEAT_WARNING → DEGRADED (remains)
 *   The HEARTBEAT_WARNING event in DEGRADED does not produce FAIL_SAFE.
 *   Escalation currently requires FAULT_ESCALATED or COMMUNICATION_LOST.
 *   Impact:       Test T05 will fail.
 *
 * =============================================================================
 * API GAPS — EVENTS MISSING FROM THE ENGINE
 * =============================================================================
 *
 * The specification requires individual positive recovery events:
 *   HEARTBEAT_OK  → missing from the API
 *   VALUE_OK      → missing from the API
 *   COMM_OK       → missing from the API
 *
 * The current engine only uses SENTINEL_EVENT_FAULT_CLEARED as a unified
 * recovery event, without distinguishing which condition was restored.
 * This prevents validation of the following rules:
 *   - restoring only HEARTBEAT_OK must not return to NOMINAL
 *   - restoring only VALUE_OK must not return to NOMINAL
 *   - restoring only COMM_OK must not return to NOMINAL
 *   - only when all three conditions are valid → NOMINAL
 *
 * Events that need to be added to the engine (sentinel_event.h):
 *   SENTINEL_EVENT_HEARTBEAT_RESTORED
 *   SENTINEL_EVENT_VALUE_RESTORED
 *   SENTINEL_EVENT_COMM_RESTORED
 *
 * While these events do not exist:
 *   - T07 (partial recovery forbidden) cannot be implemented.
 *   - T08 (full recovery allowed) cannot be implemented.
 *   - T09c (FAIL_SAFE locked against positive events) cannot be implemented.
 *   Total: 3 scenarios skipped because of API_INCOMPLETE.
 *
 * =============================================================================
 * ASSUMPTIONS MADE IN THE TESTS
 * =============================================================================
 *
 * T11 — The specification states that RESET_REQUESTED em NOMINAL must not break
 *   the system, but does not define the destination state. Assumption: the engine
 *   must return NOMINAL (preserving the current state). The test expects
 *   exactly NOMINAL.
 *
 * T12 — The specification states that SYSTEM_START em NOMINAL must not break
 *   the system, but does not define the destination state. Assumption: the engine
 *   must return NOMINAL (preserving the current state). The test expects
 *   exactly NOMINAL.
 *
 * T09 — FAULT_ESCALATED is used only as a setup mechanism to
 *   reach FAIL_SAFE through a path supported by the current engine. It does NOT replace
 *   the mandatory repeated-failure scenario in T05, which remains an independent test
 *   independent test and will continue failing while D2 exists.
 *
 * =============================================================================
 */

#include "sentinel_event.h"
#include "sentinel_state.h"
#include <stddef.h>
#include <stdio.h>

/* ---------------------------------------------------------------------------
 * Helpers
 * ---------------------------------------------------------------------------*/

/*
 * Runs an event sequence starting from INIT.
 * Validates each expected intermediate state.
 * Stops at the first divergence to avoid propagating an incorrect actual state
 * to subsequent steps, which would invalidate their preconditions.
 *
 * events[]          — list of events to apply
 * expected_states[] — expected state after each event (same length)
 * count             — number of events/states
 * test_name         — scenario name used in error messages
 *
 * Returns 0 on success and 1 on the first failure.
 */
static int run_sequence(const char *test_name,
                        const SentinelEvent *events,
                        const SentinelState *expected_states,
                        size_t count) {
    SentinelState state = SENTINEL_STATE_INIT;

    for (size_t i = 0; i < count; i++) {
        SentinelState next = sentinel_next_state(state, events[i]);
        if (next != expected_states[i]) {
            fprintf(stderr,
                    "FAIL [%s] step %zu: %s + %s -> expected %s, got %s"
                    " (aborting sequence)\n",
                    test_name, i + 1,
                    sentinel_state_to_string(state),
                    sentinel_event_to_string(events[i]),
                    sentinel_state_to_string(expected_states[i]),
                    sentinel_state_to_string(next));
            return 1;
        }
        state = next;
    }
    printf("PASS [%s]\n", test_name);
    return 0;
}

/* ---------------------------------------------------------------------------
 * Mandatory scenarios
 * ---------------------------------------------------------------------------*/

/*
 * T01 — Normal initialization
 * INIT + SYSTEM_START → NOMINAL
 */
static int test_normal_initialization(void) {
    const SentinelEvent events[] = {
        SENTINEL_EVENT_STARTUP_COMPLETE
    };
    const SentinelState expected[] = {
        SENTINEL_STATE_NOMINAL
    };
    return run_sequence("T01: normal initialization",
                        events, expected,
                        sizeof(events) / sizeof(events[0]));
}

/*
 * T02 — A single heartbeat loss transitions to DEGRADED
 * INIT → NOMINAL → DEGRADED
 */
static int test_heartbeat_timeout_degrades(void) {
    const SentinelEvent events[] = {
        SENTINEL_EVENT_STARTUP_COMPLETE,
        SENTINEL_EVENT_HEARTBEAT_WARNING
    };
    const SentinelState expected[] = {
        SENTINEL_STATE_NOMINAL,
        SENTINEL_STATE_DEGRADED
    };
    return run_sequence("T02: heartbeat timeout degrades",
                        events, expected,
                        sizeof(events) / sizeof(events[0]));
}

/*
 * T03 — An inconsistent value transitions to DEGRADED
 * INIT → NOMINAL → DEGRADED
 */
static int test_value_inconsistent_degrades(void) {
    const SentinelEvent events[] = {
        SENTINEL_EVENT_STARTUP_COMPLETE,
        SENTINEL_EVENT_INCOHERENT_PEER_STATE
    };
    const SentinelState expected[] = {
        SENTINEL_STATE_NOMINAL,
        SENTINEL_STATE_DEGRADED
    };
    return run_sequence("T03: value inconsistent degrades",
                        events, expected,
                        sizeof(events) / sizeof(events[0]));
}

/*
 * T04 — Communication loss transitions to DEGRADED
 * INIT → NOMINAL → DEGRADED
 *
 * DIVERGENCE D1: the current engine produces FAIL_SAFE for NOMINAL + COMM_LOST.
 * This test validates the specification and will fail while D1 persists.
 */
static int test_comm_lost_degrades(void) {
    const SentinelEvent events[] = {
        SENTINEL_EVENT_STARTUP_COMPLETE,
        SENTINEL_EVENT_COMMUNICATION_LOST
    };
    const SentinelState expected[] = {
        SENTINEL_STATE_NOMINAL,
        SENTINEL_STATE_DEGRADED   /* spec: DEGRADED — current engine: FAIL_SAFE (D1) */
    };
    return run_sequence("T04: comm lost degrades [spec=DEGRADED, motor=FAIL_SAFE, D1]",
                        events, expected,
                        sizeof(events) / sizeof(events[0]));
}

/*
 * T05 — Two heartbeat losses escalate to FAIL_SAFE
 * INIT → NOMINAL → DEGRADED → FAIL_SAFE
 *
 * DIVERGENCE D2: DEGRADED + HEARTBEAT_WARNING in the current engine remains
 * DEGRADED. This test validates the specification and will fail while D2 persists.
 *
 * Note: this scenario is independent from T09. FAULT_ESCALATED is not used here.
 */
static int test_double_heartbeat_timeout_escalates(void) {
    const SentinelEvent events[] = {
        SENTINEL_EVENT_STARTUP_COMPLETE,
        SENTINEL_EVENT_HEARTBEAT_WARNING,
        SENTINEL_EVENT_HEARTBEAT_WARNING
    };
    const SentinelState expected[] = {
        SENTINEL_STATE_NOMINAL,
        SENTINEL_STATE_DEGRADED,
        SENTINEL_STATE_FAIL_SAFE  /* spec: FAIL_SAFE — current engine: DEGRADED (D2) */
    };
    return run_sequence("T05: double heartbeat timeout escalates [spec=FAIL_SAFE, motor=DEGRADED, D2]",
                        events, expected,
                        sizeof(events) / sizeof(events[0]));
}

/*
 * T06 — Two communication losses escalate to FAIL_SAFE
 * INIT → NOMINAL → DEGRADED → FAIL_SAFE
 *
 * DIVERGENCE D1: the first COMM_LOST already transitions to FAIL_SAFE in the engine,
 * skipping DEGRADED. run_sequence will abort at step 2.
 */
static int test_double_comm_lost_escalates(void) {
    const SentinelEvent events[] = {
        SENTINEL_EVENT_STARTUP_COMPLETE,
        SENTINEL_EVENT_COMMUNICATION_LOST,  /* spec: DEGRADED; engine: FAIL_SAFE (D1) */
        SENTINEL_EVENT_COMMUNICATION_LOST   /* spec: FAIL_SAFE */
    };
    const SentinelState expected[] = {
        SENTINEL_STATE_NOMINAL,
        SENTINEL_STATE_DEGRADED,   /* aborts here because of D1 */
        SENTINEL_STATE_FAIL_SAFE
    };
    return run_sequence("T06: double comm lost escalates [spec=DEGRADED→FAIL_SAFE, D1]",
                        events, expected,
                        sizeof(events) / sizeof(events[0]));
}

/*
 * T07 — Premature recovery forbidden: HEARTBEAT_OK alone must not return to NOMINAL.
 *
 * API_INCOMPLETE: SENTINEL_EVENT_HEARTBEAT_RESTORED does not exist.
 * The scenario cannot be implemented correctly.
 * Required events: SENTINEL_EVENT_HEARTBEAT_RESTORED
 */
static int test_partial_recovery_not_allowed(void) {
    fprintf(stderr,
            "SKIP [T07: partial recovery not allowed] "
            "API_INCOMPLETE: SENTINEL_EVENT_HEARTBEAT_RESTORED is missing. "
            "Add it to the API to implement this scenario.\n");
    return 0;
}

/*
 * T08 — Full recovery allowed: HEARTBEAT_OK + VALUE_OK + COMM_OK → NOMINAL
 *
 * API_INCOMPLETE: individual restoration events do not exist.
 * FAULT_CLEARED does not prove that all three conditions were validated.
 * Required events: SENTINEL_EVENT_HEARTBEAT_RESTORED,
 *                      SENTINEL_EVENT_VALUE_RESTORED,
 *                      SENTINEL_EVENT_COMM_RESTORED
 */
static int test_full_recovery_allowed(void) {
    fprintf(stderr,
            "SKIP [T08: full recovery allowed] "
            "API_INCOMPLETE: SENTINEL_EVENT_HEARTBEAT_RESTORED, "
            "SENTINEL_EVENT_VALUE_RESTORED e SENTINEL_EVENT_COMM_RESTORED "
            "are missing. Add them to the API to implement this scenario.\n");
    return 0;
}

/*
 * T09 — FAIL_SAFE remains locked after positive events
 *
 * T09a: setup to reach FAIL_SAFE via FAULT_ESCALATED (supported by the engine).
 *   This sub-scenario does NOT replace T05; it only places the system
 *   in FAIL_SAFE through an isolated path testable with the current API.
 *
 * T09b: FAULT_CLEARED does not unlock FAIL_SAFE.
 *   Testable with the current API.
 *
 * T09c: HEARTBEAT_OK, VALUE_OK, and COMM_OK do not unlock FAIL_SAFE.
 *   API_INCOMPLETE: individual restoration events are missing.
 */
static int test_fail_safe_locked_by_positive_events(void) {
    /* T09a: reach FAIL_SAFE via FAULT_ESCALATED (isolated setup) */
    const SentinelEvent events_setup[] = {
        SENTINEL_EVENT_STARTUP_COMPLETE,
        SENTINEL_EVENT_HEARTBEAT_WARNING,
        SENTINEL_EVENT_FAULT_ESCALATED
    };
    const SentinelState expected_setup[] = {
        SENTINEL_STATE_NOMINAL,
        SENTINEL_STATE_DEGRADED,
        SENTINEL_STATE_FAIL_SAFE
    };
    int fails = run_sequence("T09a: reach FAIL_SAFE via FAULT_ESCALATED (isolated setup)",
                             events_setup, expected_setup,
                             sizeof(events_setup) / sizeof(events_setup[0]));

    /* T09b: FAULT_CLEARED does not unlock FAIL_SAFE */
    {
        SentinelState next = sentinel_next_state(SENTINEL_STATE_FAIL_SAFE,
                                                 SENTINEL_EVENT_FAULT_CLEARED);
        if (next != SENTINEL_STATE_FAIL_SAFE) {
            fprintf(stderr,
                    "FAIL [T09b: FAIL_SAFE not unlocked by FAULT_CLEARED]: "
                    "FAIL_SAFE + FAULT_CLEARED -> expected FAIL_SAFE, got %s\n",
                    sentinel_state_to_string(next));
            fails++;
        } else {
            printf("PASS [T09b: FAIL_SAFE not unlocked by FAULT_CLEARED]\n");
        }
    }

    /* T09c: individual positive events are unavailable */
    fprintf(stderr,
            "SKIP [T09c: FAIL_SAFE not unlocked by HEARTBEAT_OK/VALUE_OK/COMM_OK] "
            "API_INCOMPLETE: SENTINEL_EVENT_HEARTBEAT_RESTORED, "
            "SENTINEL_EVENT_VALUE_RESTORED and SENTINEL_EVENT_COMM_RESTORED are missing.\n");

    return fails;
}

/*
 * T10 — Reset from FAIL_SAFE returns to INIT
 * INIT → NOMINAL → DEGRADED → FAIL_SAFE → INIT
 */
static int test_reset_from_fail_safe(void) {
    const SentinelEvent events[] = {
        SENTINEL_EVENT_STARTUP_COMPLETE,
        SENTINEL_EVENT_HEARTBEAT_WARNING,
        SENTINEL_EVENT_FAULT_ESCALATED,
        SENTINEL_EVENT_MANUAL_RESET
    };
    const SentinelState expected[] = {
        SENTINEL_STATE_NOMINAL,
        SENTINEL_STATE_DEGRADED,
        SENTINEL_STATE_FAIL_SAFE,
        SENTINEL_STATE_INIT
    };
    return run_sequence("T10: reset from FAIL_SAFE returns to INIT",
                        events, expected,
                        sizeof(events) / sizeof(events[0]));
}

/* ---------------------------------------------------------------------------
 * Additional robustness tests
 * ---------------------------------------------------------------------------*/

/*
 * T11 — RESET_REQUESTED em NOMINAL must not break o sistema.
 *
 * Assumption: the specification does not define the destination state; we assume
 * that the engine must preserve NOMINAL. The test expects exactly NOMINAL.
 */
static int test_reset_in_nominal_is_safe(void) {
    SentinelState result = sentinel_next_state(SENTINEL_STATE_NOMINAL,
                                               SENTINEL_EVENT_MANUAL_RESET);
    if (result != SENTINEL_STATE_NOMINAL) {
        fprintf(stderr,
                "FAIL [T11: reset in NOMINAL preserves NOMINAL]: "
                "expected NOMINAL, got %s "
                "(assumption: the specification does not define the destination)\n",
                sentinel_state_to_string(result));
        return 1;
    }
    printf("PASS [T11: reset in NOMINAL preserves NOMINAL]\n");
    return 0;
}

/*
 * T12 — SYSTEM_START em NOMINAL must not break o sistema.
 *
 * Assumption: the specification does not define the destination state; we assume
 * that the engine must preserve NOMINAL. The test expects exactly NOMINAL.
 */
static int test_startup_in_nominal_is_safe(void) {
    SentinelState result = sentinel_next_state(SENTINEL_STATE_NOMINAL,
                                               SENTINEL_EVENT_STARTUP_COMPLETE);
    if (result != SENTINEL_STATE_NOMINAL) {
        fprintf(stderr,
                "FAIL [T12: startup in NOMINAL preserves NOMINAL]: "
                "expected NOMINAL, got %s "
                "(assumption: the specification does not define the destination)\n",
                sentinel_state_to_string(result));
        return 1;
    }
    printf("PASS [T12: startup in NOMINAL preserves NOMINAL]\n");
    return 0;
}

/*
 * T13 — FAULT_CLEARED alone in DEGRADED must not return to NOMINAL.
 *
 * The specification requires HEARTBEAT_OK + VALUE_OK + COMM_OK to be
 * satisfied individually. A unified FAULT_CLEARED event does not prove this.
 * The current engine returns NOMINAL after a single FAULT_CLEARED: a real divergence.
 * run_sequence will abort at step 3.
 */
static int test_fault_cleared_alone_does_not_recover(void) {
    const SentinelEvent events[] = {
        SENTINEL_EVENT_STARTUP_COMPLETE,
        SENTINEL_EVENT_HEARTBEAT_WARNING,
        SENTINEL_EVENT_FAULT_CLEARED
    };
    const SentinelState expected[] = {
        SENTINEL_STATE_NOMINAL,
        SENTINEL_STATE_DEGRADED,
        SENTINEL_STATE_DEGRADED  /* spec: individual conditions required; engine: NOMINAL */
    };
    return run_sequence("T13: FAULT_CLEARED alone must not recover [engine returns NOMINAL, API_INCOMPLETE]",
                        events, expected,
                        sizeof(events) / sizeof(events[0]));
}

/* ---------------------------------------------------------------------------
 * main
 * ---------------------------------------------------------------------------*/

int main(void) {
    int fails = 0;
    int skips = 0;

    puts("=== SNT-TEST-001: Sentinel Fault Validation Suite ===\n");

    fails += test_normal_initialization();
    fails += test_heartbeat_timeout_degrades();
    fails += test_value_inconsistent_degrades();
    fails += test_comm_lost_degrades();
    fails += test_double_heartbeat_timeout_escalates();
    fails += test_double_comm_lost_escalates();

    skips++; test_partial_recovery_not_allowed(); /* T07 */
    skips++; test_full_recovery_allowed();         /* T08 */

    fails += test_fail_safe_locked_by_positive_events(); /* T09a, T09b, T09c(skip) */
    skips++;  /* T09c counted here */

    fails += test_reset_from_fail_safe();
    fails += test_reset_in_nominal_is_safe();
    fails += test_startup_in_nominal_is_safe();
    fails += test_fault_cleared_alone_does_not_recover();

    puts("\n=== Result ===");
    printf("Skipped (API_INCOMPLETE): %d (T07, T08, T09c)\n", skips);
    if (fails > 0) {
        fprintf(stderr,
                "FAIL: %d test(s) failed.\n"
                "Active divergences: D1 (NOMINAL+COMM_LOST→FAIL_SAFE), "
                "D2 (DEGRADED+HEARTBEAT_WARNING does not escalate), "
                "permissive recovery through FAULT_CLEARED.\n",
                fails);
        return 1;
    }
    puts("All tests passed.");
    return 0;