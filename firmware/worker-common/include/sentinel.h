#ifndef SENTINEL_H
#define SENTINEL_H

#include "sentinel_event.h"
#include "sentinel_state.h"
#include <stdbool.h>

struct Sentinel {
	SentinelState state;
	bool heartbeat_recovered;
	bool value_recovered;
	bool comm_recovered;
};

void sentinel_init(struct Sentinel *sentinel);

void sentinel_apply_event(struct Sentinel *sentinel, SentinelEvent event);

SentinelState sentinel_get_state(const struct Sentinel *sentinel);

#endif // !SENTINEL_H
