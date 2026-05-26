#ifndef SENTINEL_STATE_H
#define SENTINEL_STATE_H

#include "sentinel_event.h"

typedef enum {
	SENTINEL_STATE_INIT,
	SENTINEL_STATE_NOMINAL,
	SENTINEL_STATE_DEGRADED,
	SENTINEL_STATE_FAIL_SAFE
} SentinelState;

const char *sentinel_state_to_string(SentinelState state);

SentinelState sentinel_next_state(SentinelState current, SentinelEvent event);

#endif // !SENTINEL_STATE_H
