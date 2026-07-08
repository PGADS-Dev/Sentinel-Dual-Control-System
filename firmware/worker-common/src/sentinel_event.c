#include "sentinel_event.h"
#include "sentinel.h"
#include "sentinel_state.h"

void sentinel_apply_event(struct Sentinel *sentinel, SentinelEvent event) {
	if (event == SENTINEL_EVENT_HEARTBEAT_OK) {
		sentinel->heartbeat_recovered = true;
	} else if (event == SENTINEL_EVENT_VALUE_OK) {
		sentinel->value_recovered = true;
	} else if (event == SENTINEL_EVENT_COMM_OK) {
		sentinel->comm_recovered = true;
	}

	if (sentinel->heartbeat_recovered && sentinel->value_recovered && sentinel->comm_recovered) {
		sentinel->state = SENTINEL_STATE_NOMINAL;
	}
}
