#include "sentinel.h"
#include "sentinel_event.h"
#include "sentinel_state.h"
#include <assert.h>
#include <stdlib.h>

void sentinel_reset_flags(struct Sentinel *sentinel) {
	sentinel->heartbeat_recovered = false;
	sentinel->value_recovered = false;
	sentinel->comm_recovered = false;
}

void sentinel_apply_event(struct Sentinel *sentinel, SentinelEvent event) {
	SentinelState old_state = sentinel->state;

	if (old_state == SENTINEL_STATE_DEGRADED) {
		if (event == SENTINEL_EVENT_HEARTBEAT_OK) {
			sentinel->heartbeat_recovered = true;
		} else if (event == SENTINEL_EVENT_VALUE_OK) {
			sentinel->value_recovered = true;
		} else if (event == SENTINEL_EVENT_COMM_OK) {
			sentinel->comm_recovered = true;
		}

		if (sentinel->heartbeat_recovered && sentinel->value_recovered && sentinel->comm_recovered) {
			sentinel->state = SENTINEL_STATE_NOMINAL;
			sentinel_reset_flags(sentinel);
			return;
		}
	}

	SentinelState new_state = sentinel_next_state(old_state, event);

	if (new_state != old_state) {
		sentinel->state = new_state;

		if (new_state == SENTINEL_STATE_DEGRADED || new_state == SENTINEL_STATE_FAIL_SAFE ||
		    new_state == SENTINEL_STATE_INIT || new_state == SENTINEL_STATE_NOMINAL) {
			sentinel_reset_flags(sentinel);
		}
	}
}

void sentinel_init(struct Sentinel *sentinel) {
	assert(sentinel != NULL);

	sentinel->state = SENTINEL_STATE_INIT;
	sentinel_reset_flags(sentinel);
}

SentinelState sentinel_get_state(const struct Sentinel *sentinel) { return sentinel->state; }
