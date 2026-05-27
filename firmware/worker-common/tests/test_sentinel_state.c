#include "sentinel_event.h"
#include "sentinel_state.h"
#include <stdio.h>

static int assert_transition(SentinelState current, SentinelEvent event, SentinelState expected) {
	SentinelState actual = sentinel_next_state(current, event);

	if (actual != expected) {
		fprintf(stderr, "FAIL: %s + %s -> expected %s, got %s\n", sentinel_state_to_string(current),
		        sentinel_event_to_string(event), sentinel_state_to_string(expected), sentinel_state_to_string(actual));
		return 1;
	}

	return 0;
}

/**
 * Sentinel state machine transitions covered:
 * - INIT + STARTUP_COMPLETE -> NOMINAL
 * - INIT + COMMUNICATION_LOST -> FAIL_SAFE
 * - INIT + FAULT_ESCALATED -> FAIL_SAFE
 *
 * - NOMINAL + HEARTBEAT_WARNING -> DEGRADED
 * - NOMINAL + INCOHERENT_PEER_STATE -> DEGRADED
 * - NOMINAL + COMMUNICATION_LOST -> FAIL_SAFE
 * - NOMINAL + FAULT_ESCALATED -> FAIL_SAFE
 *
 * - DEGRADED + FAULT_CLEARED -> NOMINAL
 * - DEGRADED + COMMUNICATION_LOST -> FAIL_SAFE
 * - DEGRADED + FAULT_ESCALATED -> FAIL_SAFE
 *
 * - FAIL_SAFE + MANUAL_RESET -> INIT
 * - UNKNOWN_STATE + STARTUP_COMPLETE -> FAIL_SAFE
 */
int main(void) {
	int failures = 0;

	failures += assert_transition(SENTINEL_STATE_INIT, SENTINEL_EVENT_STARTUP_COMPLETE, SENTINEL_STATE_NOMINAL);
	failures += assert_transition(SENTINEL_STATE_INIT, SENTINEL_EVENT_COMMUNICATION_LOST, SENTINEL_STATE_FAIL_SAFE);
	failures += assert_transition(SENTINEL_STATE_INIT, SENTINEL_EVENT_FAULT_ESCALATED, SENTINEL_STATE_FAIL_SAFE);

	failures += assert_transition(SENTINEL_STATE_NOMINAL, SENTINEL_EVENT_HEARTBEAT_WARNING, SENTINEL_STATE_DEGRADED);
	failures +=
	    assert_transition(SENTINEL_STATE_NOMINAL, SENTINEL_EVENT_INCOHERENT_PEER_STATE, SENTINEL_STATE_DEGRADED);
	failures += assert_transition(SENTINEL_STATE_NOMINAL, SENTINEL_EVENT_COMMUNICATION_LOST, SENTINEL_STATE_FAIL_SAFE);
	failures += assert_transition(SENTINEL_STATE_NOMINAL, SENTINEL_EVENT_FAULT_ESCALATED, SENTINEL_STATE_FAIL_SAFE);

	failures += assert_transition(SENTINEL_STATE_DEGRADED, SENTINEL_EVENT_FAULT_CLEARED, SENTINEL_STATE_NOMINAL);
	failures += assert_transition(SENTINEL_STATE_DEGRADED, SENTINEL_EVENT_COMMUNICATION_LOST, SENTINEL_STATE_FAIL_SAFE);
	failures += assert_transition(SENTINEL_STATE_DEGRADED, SENTINEL_EVENT_FAULT_ESCALATED, SENTINEL_STATE_FAIL_SAFE);

	failures += assert_transition(SENTINEL_STATE_FAIL_SAFE, SENTINEL_EVENT_MANUAL_RESET, SENTINEL_STATE_INIT);

	failures += assert_transition((SentinelState)99, SENTINEL_EVENT_STARTUP_COMPLETE, SENTINEL_STATE_FAIL_SAFE);

	if (failures != 0) {
		fprintf(stderr, "%d Sentinel state machine test(s) failed.\n", failures);
		return 1;
	}

	puts("All Sentinel state machine tests passed.");

	return 0;
}
