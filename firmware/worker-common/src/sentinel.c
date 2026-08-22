#include "sentinel.h"
#include "sentinel_event.h"
#include "sentinel_state.h"
#include <assert.h>
#include <stddef.h>

static inline void sentinel_fault_set_clear(SentinelFaultSet *set) { set->bits = SENTINEL_FAULT_NONE; }

static inline void sentinel_fault_set_add(SentinelFaultSet *set, SentinelFault fault) { set->bits |= (uint32_t)fault; }

static inline void sentinel_fault_set_remove(SentinelFaultSet *set, SentinelFault fault) {
	set->bits &= ~(uint32_t)fault;
}

static inline bool sentinel_fault_set_contains(const SentinelFaultSet *set, SentinelFault fault) {
	return (set->bits & (uint32_t)fault) != 0u;
}

static inline bool sentinel_fault_set_is_empty(const SentinelFaultSet *set) { return set->bits == 0u; }

void sentinel_apply_event(struct Sentinel *sentinel, SentinelEvent event) {
	assert(sentinel != NULL);
	SentinelState old_state = sentinel->state;

	switch (event) {
	case SENTINEL_EVENT_HEARTBEAT_TIMEOUT:
		sentinel_fault_set_add(&sentinel->active_faults, SENTINEL_FAULT_HEARTBEAT_LOST);
		break;

	case SENTINEL_EVENT_HEARTBEAT_OK:
		sentinel_fault_set_remove(&sentinel->active_faults, SENTINEL_FAULT_HEARTBEAT_LOST);
		break;
	case SENTINEL_EVENT_INVALID:
	case SENTINEL_EVENT_COMM_LOST:
	case SENTINEL_EVENT_COMM_OK:
	case SENTINEL_EVENT_FAULT_ESCALATED:
	case SENTINEL_EVENT_RESET_REQUESTED:
	case SENTINEL_EVENT_SYSTEM_START:
	case SENTINEL_EVENT_VALUE_INCONSISTENT:
	case SENTINEL_EVENT_VALUE_OK:
		break;
	}

	if (old_state == SENTINEL_STATE_DEGRADED) {
		if (event == SENTINEL_EVENT_HEARTBEAT_OK) {
			// sentinel->heartbeat_recovered = true;
		} else if (event == SENTINEL_EVENT_VALUE_OK) {
			// sentinel->value_recovered = true;
		} else if (event == SENTINEL_EVENT_COMM_OK) {
			// sentinel->comm_recovered = true;
		}

		// if (sentinel->heartbeat_recovered && sentinel->value_recovered && sentinel->comm_recovered) {
		// 	sentinel->state = SENTINEL_STATE_NOMINAL;
		// 	sentinel_reset_flags(sentinel);
		// 	return;
		// }
	}

	SentinelState new_state = sentinel_next_state(old_state, event);

	if (new_state != old_state) {
		sentinel->state = new_state;

		if (new_state == SENTINEL_STATE_DEGRADED || new_state == SENTINEL_STATE_FAIL_SAFE ||
		    new_state == SENTINEL_STATE_INIT || new_state == SENTINEL_STATE_NOMINAL) {
			// sentinel_reset_flags(sentinel);
		}
	}
}

void sentinel_init(struct Sentinel *sentinel) {
	assert(sentinel != NULL);

	sentinel->state = SENTINEL_STATE_INIT;
	sentinel->active_faults.bits = SENTINEL_FAULT_NONE;
}

SentinelState sentinel_get_state(const struct Sentinel *sentinel) { return sentinel->state; }
