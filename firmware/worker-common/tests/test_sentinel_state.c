#include "sentinel.h"
#include "sentinel_event.h"
#include "sentinel_state.h"
#include <stdio.h>
#include <stdlib.h>

static int assert_transition(SentinelState current, SentinelEvent event, SentinelState expected) {
	SentinelState actual = sentinel_next_state(current, event);

	if (actual != expected) {
		fprintf(stderr, "[FAIL]: %s + %s -> expected %s, got %s\n", sentinel_state_to_string(current),
		        sentinel_event_to_string(event), sentinel_state_to_string(expected), sentinel_state_to_string(actual));
		return 1;
	}

	fprintf(stdout, "[SUCCEED]: %s + %s -> expected %s\n", sentinel_state_to_string(current),
	        sentinel_event_to_string(event), sentinel_state_to_string(expected));
	return 0;
}

static int assert_state(const struct Sentinel *sentinel, SentinelState expected) {
	SentinelState actual = sentinel_get_state(sentinel);
	SentinelState current = sentinel->state;

	if (actual != expected) {
		fprintf(stderr, "[FAIL]: %s -> expected %s, got %s\n", sentinel_state_to_string(current),
		        sentinel_state_to_string(expected), sentinel_state_to_string(actual));
		return 1;
	}

	fprintf(stdout, "[SUCCEED]: %s -> expected %s\n", sentinel_state_to_string(current),
	        sentinel_state_to_string(expected));
	return 0;
}

/**
 * Sentinel state machine transitions covered:
 * - INIT			+ SYSTEM_START			-> NOMINAL
 *
 * - NOMINAL		+ HEARTBEAT_TIMEOUT		-> DEGRADED
 * - NOMINAL		+ VALUE_INCONSISTENT	-> DEGRADED
 * - NOMINAL		+ COMM_LOST				-> DEGRADED
 *
 * - DEGRADED		+ HEARTBEAT_TIMEOUT		-> FAIL_SAFE
 * - DEGRADED		+ VALUE_INCONSISTENT	-> FAIL_SAFE
 * - DEGRADED		+ COMM_LOST				-> FAIL_SAFE
 *
 * - DEGRADED		+ HEARTBEAT_OK			-> DEGRADED
 * - DEGRADED		+ VALUE_OK				-> DEGRADED
 * - DEGRADED		+ COMM_OK				-> DEGRADED
 *
 * - FAIL_SAFE		+ RESET_REQUESTED		-> INIT
 * - FAIL_SAFE		+ HEARTBEAT_OK			-> FAIL_SAFE
 * - FAIL_SAFE		+ VALUE_OK				-> FAIL_SAFE
 * - FAIL_SAFE		+ COMM_OK				-> FAIL_SAFE
 *
 * - UNKNOWN_STATE	+ HEARTBEAT_OK			-> FAIL_SAFE
 * - UNKNOWN_STATE	+ UNKNOWN_EVENT			-> FAIL_SAFE
 */
int main(void) {
	int failures = 0;

	struct Sentinel sentinel;

	sentinel_init(&sentinel);
	failures += assert_state(&sentinel, SENTINEL_STATE_INIT);

	failures += assert_transition(SENTINEL_STATE_INIT, SENTINEL_EVENT_SYSTEM_START, SENTINEL_STATE_NOMINAL);

	failures += assert_transition(SENTINEL_STATE_NOMINAL, SENTINEL_EVENT_HEARTBEAT_TIMEOUT, SENTINEL_STATE_DEGRADED);
	failures += assert_transition(SENTINEL_STATE_NOMINAL, SENTINEL_EVENT_VALUE_INCONSISTENT, SENTINEL_STATE_DEGRADED);
	failures += assert_transition(SENTINEL_STATE_NOMINAL, SENTINEL_EVENT_COMM_LOST, SENTINEL_STATE_DEGRADED);

	failures += assert_transition(SENTINEL_STATE_DEGRADED, SENTINEL_EVENT_HEARTBEAT_TIMEOUT, SENTINEL_STATE_FAIL_SAFE);
	failures += assert_transition(SENTINEL_STATE_DEGRADED, SENTINEL_EVENT_VALUE_INCONSISTENT, SENTINEL_STATE_FAIL_SAFE);
	failures += assert_transition(SENTINEL_STATE_DEGRADED, SENTINEL_EVENT_COMM_LOST, SENTINEL_STATE_FAIL_SAFE);

	failures += assert_transition(SENTINEL_STATE_DEGRADED, SENTINEL_EVENT_HEARTBEAT_OK, SENTINEL_STATE_DEGRADED);
	failures += assert_transition(SENTINEL_STATE_DEGRADED, SENTINEL_EVENT_VALUE_OK, SENTINEL_STATE_DEGRADED);
	failures += assert_transition(SENTINEL_STATE_DEGRADED, SENTINEL_EVENT_COMM_OK, SENTINEL_STATE_DEGRADED);

	failures += assert_transition(SENTINEL_STATE_FAIL_SAFE, SENTINEL_EVENT_RESET_REQUESTED, SENTINEL_STATE_INIT);
	failures += assert_transition(SENTINEL_STATE_FAIL_SAFE, SENTINEL_EVENT_HEARTBEAT_OK, SENTINEL_STATE_FAIL_SAFE);
	failures += assert_transition(SENTINEL_STATE_FAIL_SAFE, SENTINEL_EVENT_VALUE_OK, SENTINEL_STATE_FAIL_SAFE);
	failures += assert_transition(SENTINEL_STATE_FAIL_SAFE, SENTINEL_EVENT_COMM_OK, SENTINEL_STATE_FAIL_SAFE);

	failures += assert_transition((SentinelState)99, SENTINEL_EVENT_HEARTBEAT_OK, SENTINEL_STATE_FAIL_SAFE);
	failures += assert_transition((SentinelState)99, (SentinelEvent)99, SENTINEL_STATE_FAIL_SAFE);

	sentinel_init(&sentinel);
	sentinel_apply_event(&sentinel, SENTINEL_EVENT_SYSTEM_START);
	failures += assert_state(&sentinel, SENTINEL_STATE_NOMINAL);

	sentinel_init(&sentinel);
	sentinel_apply_event(&sentinel, SENTINEL_EVENT_SYSTEM_START);
	sentinel_apply_event(&sentinel, SENTINEL_EVENT_HEARTBEAT_TIMEOUT);
	failures += assert_state(&sentinel, SENTINEL_STATE_DEGRADED);

	sentinel_init(&sentinel);
	sentinel_apply_event(&sentinel, SENTINEL_EVENT_SYSTEM_START);
	sentinel_apply_event(&sentinel, SENTINEL_EVENT_HEARTBEAT_TIMEOUT);
	sentinel_apply_event(&sentinel, SENTINEL_EVENT_HEARTBEAT_OK);
	failures += assert_state(&sentinel, SENTINEL_STATE_DEGRADED);

	sentinel_init(&sentinel);
	sentinel_apply_event(&sentinel, SENTINEL_EVENT_SYSTEM_START);
	sentinel_apply_event(&sentinel, SENTINEL_EVENT_HEARTBEAT_TIMEOUT);
	sentinel_apply_event(&sentinel, SENTINEL_EVENT_HEARTBEAT_OK);
	sentinel_apply_event(&sentinel, SENTINEL_EVENT_VALUE_OK);
	failures += assert_state(&sentinel, SENTINEL_STATE_DEGRADED);

	sentinel_init(&sentinel);
	sentinel_apply_event(&sentinel, SENTINEL_EVENT_SYSTEM_START);
	sentinel_apply_event(&sentinel, SENTINEL_EVENT_HEARTBEAT_TIMEOUT);
	sentinel_apply_event(&sentinel, SENTINEL_EVENT_HEARTBEAT_OK);
	sentinel_apply_event(&sentinel, SENTINEL_EVENT_VALUE_OK);
	sentinel_apply_event(&sentinel, SENTINEL_EVENT_COMM_OK);
	failures += assert_state(&sentinel, SENTINEL_STATE_NOMINAL);

	sentinel_init(&sentinel);
	sentinel_apply_event(&sentinel, SENTINEL_EVENT_SYSTEM_START);
	sentinel_apply_event(&sentinel, SENTINEL_EVENT_HEARTBEAT_TIMEOUT);
	sentinel_apply_event(&sentinel, SENTINEL_EVENT_HEARTBEAT_TIMEOUT);
	failures += assert_state(&sentinel, SENTINEL_STATE_FAIL_SAFE);

	// This test is just to prove that the Fail Safe stay locked
	sentinel_init(&sentinel);
	sentinel_apply_event(&sentinel, SENTINEL_EVENT_SYSTEM_START);
	sentinel_apply_event(&sentinel, SENTINEL_EVENT_HEARTBEAT_TIMEOUT);
	sentinel_apply_event(&sentinel, SENTINEL_EVENT_HEARTBEAT_TIMEOUT);
	sentinel_apply_event(&sentinel, SENTINEL_EVENT_HEARTBEAT_OK);
	sentinel_apply_event(&sentinel, SENTINEL_EVENT_VALUE_OK);
	sentinel_apply_event(&sentinel, SENTINEL_EVENT_COMM_OK);
	failures += assert_state(&sentinel, SENTINEL_STATE_FAIL_SAFE);

	sentinel_init(&sentinel);
	sentinel_apply_event(&sentinel, SENTINEL_EVENT_SYSTEM_START);
	sentinel_apply_event(&sentinel, SENTINEL_EVENT_HEARTBEAT_TIMEOUT);
	sentinel_apply_event(&sentinel, SENTINEL_EVENT_HEARTBEAT_TIMEOUT);
	sentinel_apply_event(&sentinel, SENTINEL_EVENT_RESET_REQUESTED);
	failures += assert_state(&sentinel, SENTINEL_STATE_INIT);

	if (failures != 0) {
		printf("%d Sentinel state machine tests failed.\n", failures);
		return EXIT_FAILURE;
	}

	puts("All Sentinel state machine tests passed.");

	return EXIT_SUCCESS;
}
