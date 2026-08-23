#ifndef SENTINEL_H
#define SENTINEL_H

#include "sentinel_event.h"
#include "sentinel_fault.h"
#include "sentinel_state.h"
#include <stdint.h>
#include <stdbool.h>

struct Sentinel {
	SentinelState state;
	SentinelFaultSet active_faults;
};

void sentinel_init(struct Sentinel *sentinel);

void sentinel_apply_event(struct Sentinel *sentinel, SentinelEvent event);

SentinelState sentinel_get_state(const struct Sentinel *sentinel);

SentinelFaultSet sentinel_get_active_faults(const struct Sentinel *sentinel);

bool sentinel_has_fault(const struct Sentinel *sentinel, SentinelFault fault);

#endif // !SENTINEL_H
