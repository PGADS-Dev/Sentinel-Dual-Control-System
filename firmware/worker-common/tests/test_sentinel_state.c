#include "sentinel_event.h"
#include "sentinel_state.h"
#include <stdio.h>

#define ASSERT_STATE(actual, expected)                                                                                 \
	do {                                                                                                               \
		if ((actual) != (expected)) {                                                                                  \
			fprintf(stderr, "[\033[0;31mFAILED\033[0m]: expected %d, got %d at %s:%d\n", (expected), (actual),         \
			        __FILE__, __LINE__);                                                                               \
		} else {                                                                                                       \
			fprintf(stdout, "[\033[0;32mSUCCEED\033[0m]: got %d\n", (actual));                                         \
		}                                                                                                              \
	} while (0)


/**
 * Here are the tests currently being run:
 * - INIT + STARTUP_COMPLETE -> NOMINAL
 * - INIT + COMMUNICATION_LOST -> FAIL_SAFE
 * - INIT + FAULT_ESCALATED -> FAIL_SAFE

 * - NOMINAL + HEARTBEAT_WARNING -> DEGRADED
 * - NOMINAL + INCOHERENT_PEER_STATE -> DEGRADED
 * - NOMINAL + COMMUNICATION_LOST -> FAIL_SAFE
 * - NOMINAL + FAULT_ESCALATED -> FAIL_SAFE

 * - DEGRADED + FAULT_CLEARED -> NOMINAL
 * - DEGRADED + COMMUNICATION_LOST -> FAIL_SAFE
 * - DEGRADED + FAULT_ESCALATED -> FAIL_SAFE

 * - FAIL_SAFE + MANUAL_RESET -> INIT
 */
int main(void) {
	SentinelState state = SENTINEL_STATE_INIT;
	ASSERT_STATE(sentinel_next_state(state, SENTINEL_EVENT_STARTUP_COMPLETE), SENTINEL_STATE_NOMINAL);
	ASSERT_STATE(sentinel_next_state(state, SENTINEL_EVENT_COMMUNICATION_LOST), SENTINEL_STATE_FAIL_SAFE);
	ASSERT_STATE(sentinel_next_state(state, SENTINEL_EVENT_FAULT_ESCALATED), SENTINEL_STATE_FAIL_SAFE);

	state = SENTINEL_STATE_NOMINAL;
	ASSERT_STATE(sentinel_next_state(state, SENTINEL_EVENT_HEARTBEAT_WARNING), SENTINEL_STATE_DEGRADED);
	ASSERT_STATE(sentinel_next_state(state, SENTINEL_EVENT_INCOHERENT_PEER_STATE), SENTINEL_STATE_DEGRADED);
	ASSERT_STATE(sentinel_next_state(state, SENTINEL_EVENT_COMMUNICATION_LOST), SENTINEL_STATE_FAIL_SAFE);
	ASSERT_STATE(sentinel_next_state(state, SENTINEL_EVENT_FAULT_ESCALATED), SENTINEL_STATE_FAIL_SAFE);

	state = SENTINEL_STATE_DEGRADED;
	ASSERT_STATE(sentinel_next_state(state, SENTINEL_EVENT_FAULT_CLEARED), SENTINEL_STATE_NOMINAL);
	ASSERT_STATE(sentinel_next_state(state, SENTINEL_EVENT_COMMUNICATION_LOST), SENTINEL_STATE_FAIL_SAFE);
	ASSERT_STATE(sentinel_next_state(state, SENTINEL_EVENT_FAULT_ESCALATED), SENTINEL_STATE_FAIL_SAFE);

	state = SENTINEL_STATE_FAIL_SAFE;
	ASSERT_STATE(sentinel_next_state(state, SENTINEL_EVENT_MANUAL_RESET), SENTINEL_STATE_INIT);

	return 0;
}
