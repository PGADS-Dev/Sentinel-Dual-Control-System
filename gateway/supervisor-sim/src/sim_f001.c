#include "sentinel_event.h"
#include "sentinel_state.h"
#include <stdint.h>
#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>

#include <sys/time.h>

/**
 * @brief get actual time in milliseconds
 *
 * @return The actual time in milliseconds
 */
long long timeInMilliseconds(void) {
	struct timeval time;

	gettimeofday(&time, NULL);
	return (((long long)time.tv_sec) * 1000) + (time.tv_usec / 1000);
}

/**
 * @brief Apply a state in response of an event.
 *
 * @param timestamp_ms The actual time in ms
 * @param worker_id The id of the worker
 * @param state The current state of the worker
 * @param event The event happening to the worker
 * @return The new state in response to the event
 */
SentinelState apply_and_trace(uint32_t timestamp_ms, const char* worker_id, SentinelState state, SentinelEvent event) {
	SentinelState new_state = sentinel_next_state(state, event);
	printf("[%u ms] worker=%c event=%s state=%s → %s\n", timestamp_ms, *worker_id, sentinel_event_to_string(event),
	       sentinel_state_to_string(state), sentinel_state_to_string(new_state));
	return new_state;
}

int main() {
	const char worker_id = 'a';
	SentinelState worker_state = SENTINEL_STATE_INIT;
	sleep(1);
	worker_state = apply_and_trace(
		timeInMilliseconds(),
		&worker_id,
		worker_state,
		SENTINEL_EVENT_STARTUP_COMPLETE
	);
	sleep(3);
	worker_state = apply_and_trace(
		timeInMilliseconds(),
		&worker_id,
		worker_state,
		SENTINEL_EVENT_HEARTBEAT_WARNING
	);
	sleep(1);
	worker_state = apply_and_trace(
		timeInMilliseconds(),
		&worker_id,
		worker_state,
		SENTINEL_EVENT_FAULT_ESCALATED
	);
	return 0;
}
