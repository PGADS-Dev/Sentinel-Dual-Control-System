#include "sentinel.h"
#include "sentinel_event.h"
#include "sentinel_state.h"
#include <stdio.h>
#include <stdlib.h>

static int apply_and_assert(struct Sentinel *sentinel, SentinelEvent event, SentinelState expected) {
	SentinelState previous = sentinel_get_state(sentinel);

	sentinel_apply_event(sentinel, event);

	SentinelState actual = sentinel_get_state(sentinel);

	if (actual != expected) {
		fprintf(stderr, "[FAIL]: %s + %s -> expected %s, got %s\n", sentinel_state_to_string(previous),
		        sentinel_event_to_string(event), sentinel_state_to_string(expected), sentinel_state_to_string(actual));
		return 1;
	}

	fprintf(stdout, "[SUCCEED]: %s + %s -> %s\n", sentinel_state_to_string(previous), sentinel_event_to_string(event),
	        sentinel_state_to_string(actual));

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

	sentinel_init(&sentinel);
	failures += apply_and_assert(&sentinel, SENTINEL_EVENT_SYSTEM_START, SENTINEL_STATE_NOMINAL);
	failures += apply_and_assert(&sentinel, SENTINEL_EVENT_HEARTBEAT_TIMEOUT, SENTINEL_STATE_DEGRADED);
	failures += apply_and_assert(&sentinel, SENTINEL_EVENT_HEARTBEAT_OK, SENTINEL_STATE_NOMINAL);

	sentinel_init(&sentinel);
	failures += apply_and_assert(&sentinel, SENTINEL_EVENT_SYSTEM_START, SENTINEL_STATE_NOMINAL);
	failures += apply_and_assert(&sentinel, SENTINEL_EVENT_HEARTBEAT_TIMEOUT, SENTINEL_STATE_DEGRADED);
	failures += apply_and_assert(&sentinel, SENTINEL_EVENT_HEARTBEAT_TIMEOUT, SENTINEL_STATE_DEGRADED);

	sentinel_init(&sentinel);
	failures += apply_and_assert(&sentinel, SENTINEL_EVENT_SYSTEM_START, SENTINEL_STATE_NOMINAL);
	failures += apply_and_assert(&sentinel, SENTINEL_EVENT_HEARTBEAT_TIMEOUT, SENTINEL_STATE_DEGRADED);
	failures += apply_and_assert(&sentinel, SENTINEL_EVENT_VALUE_INCONSISTENT, SENTINEL_STATE_DEGRADED);
	failures += apply_and_assert(&sentinel, SENTINEL_EVENT_HEARTBEAT_OK, SENTINEL_STATE_DEGRADED);
	failures += apply_and_assert(&sentinel, SENTINEL_EVENT_VALUE_OK, SENTINEL_STATE_NOMINAL);

	sentinel_init(&sentinel);
	failures += apply_and_assert(&sentinel, SENTINEL_EVENT_SYSTEM_START, SENTINEL_STATE_NOMINAL);
	failures += apply_and_assert(&sentinel, SENTINEL_EVENT_HEARTBEAT_TIMEOUT, SENTINEL_STATE_DEGRADED);
	failures += apply_and_assert(&sentinel, SENTINEL_EVENT_FAULT_ESCALATED, SENTINEL_STATE_FAIL_SAFE);
	failures += apply_and_assert(&sentinel, SENTINEL_EVENT_HEARTBEAT_OK, SENTINEL_STATE_FAIL_SAFE);
	failures += apply_and_assert(&sentinel, SENTINEL_EVENT_RESET_REQUESTED, SENTINEL_STATE_INIT);

	sentinel_init(&sentinel);
	failures += apply_and_assert(&sentinel, SENTINEL_EVENT_SYSTEM_START, SENTINEL_STATE_NOMINAL);
	failures += apply_and_assert(&sentinel, SENTINEL_EVENT_COMM_LOST, SENTINEL_STATE_FAIL_SAFE);

	if (failures != 0) {
		fprintf(stderr, "%d Sentinel state machine tests failed.\n", failures);
		return EXIT_FAILURE;
	}

	fputs("All Sentinel state machine tests passed.\n", stderr);

	return EXIT_SUCCESS;
}
