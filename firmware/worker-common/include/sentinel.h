#ifndef SENTINEL_H
#define SENTINEL_H

#include "sentinel_state.h"

struct Sentinel {
	SentinelState state;
	int heartbeat_recovered;
	int value_recovered;
	int comm_recovered;
};

void sentinel_init(struct Sentinel sentinel);

void sentinel_apply_event(struct Sentinel sentinel, SentinelState state);

SentinelState sentinel_get_state(struct Sentinel sentinel);

#endif // !SENTINEL_H
