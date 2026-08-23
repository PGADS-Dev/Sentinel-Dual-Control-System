#include "sentinel.h"
#include "sentinel_event.h"
#include "sentinel_fault.h"
#include "sentinel_state.h"
#include <assert.h>
#include <stddef.h>

static inline void sentinel_fault_set_clear(SentinelFaultSet *set) {
	set->bits = SENTINEL_FAULT_NONE;
}

static inline void sentinel_fault_set_add(SentinelFaultSet *set, SentinelFault fault) {
	set->bits |= (uint32_t)fault;
}

static inline void sentinel_fault_set_remove(SentinelFaultSet *set, SentinelFault fault) {
	set->bits &= ~(uint32_t)fault;
}

static inline bool sentinel_fault_set_contains(const SentinelFaultSet *set, SentinelFault fault) {
	return (set->bits & (uint32_t)fault) != 0u;
}

static inline bool sentinel_fault_set_is_empty(const SentinelFaultSet *set) {
	return set->bits == 0u;
}

static bool sentinel_state_is_valid(SentinelState state) {
	switch (state) {
	case SENTINEL_STATE_INIT:
	case SENTINEL_STATE_NOMINAL:
	case SENTINEL_STATE_DEGRADED:
	case SENTINEL_STATE_FAIL_SAFE:
		return true;
	default:
		return false;
	}
}

static void sentinel_clear_fault_and_recover(struct Sentinel *sentinel, SentinelFault fault) {
	bool was_active = sentinel_fault_set_contains(&sentinel->active_faults, fault);

	sentinel_fault_set_remove(&sentinel->active_faults, fault);

	if (sentinel->state == SENTINEL_STATE_DEGRADED && was_active &&
	    sentinel_fault_set_is_empty(&sentinel->active_faults)) {
		sentinel->state = SENTINEL_STATE_NOMINAL;
	}
}

void sentinel_apply_event(struct Sentinel *sentinel, SentinelEvent event) {
	assert(sentinel != NULL);

	if (!sentinel_state_is_valid(sentinel->state)) {
		sentinel->state = SENTINEL_STATE_FAIL_SAFE;
		return;
	}

	/*
	 * FAIL_SAFE is latched.
	 *
	 * New fault information may still be recorded, but recovery events
	 * cannot clear faults. Only RESET_REQUESTED unlocks the worker and
	 * reinitializes fault tracking
	 */
	if (sentinel->state == SENTINEL_STATE_FAIL_SAFE) {
		switch (event) {
		case SENTINEL_EVENT_HEARTBEAT_TIMEOUT:
			sentinel_fault_set_add(
			    &sentinel->active_faults,
			    SENTINEL_FAULT_HEARTBEAT_LOST
			);
			break;
		case SENTINEL_EVENT_VALUE_INCONSISTENT:
			sentinel_fault_set_add(
			    &sentinel->active_faults,
			    SENTINEL_FAULT_INCOHERENT_PEER_STATE
			);
			break;
		case SENTINEL_EVENT_COMM_LOST:
			sentinel_fault_set_add(
			    &sentinel->active_faults,
			    SENTINEL_FAULT_COMMUNICATION_LOST
			);
			break;
		case SENTINEL_EVENT_RESET_REQUESTED:
			sentinel_fault_set_clear(&sentinel->active_faults);
			sentinel->state = SENTINEL_STATE_INIT;
			break;
		case SENTINEL_EVENT_INVALID:
		case SENTINEL_EVENT_SYSTEM_START:
		case SENTINEL_EVENT_HEARTBEAT_OK:
		case SENTINEL_EVENT_VALUE_OK:
		case SENTINEL_EVENT_COMM_OK:
		case SENTINEL_EVENT_FAULT_ESCALATED:
			break;
		default:
			break;
		}
		return;
	}

	switch (event) {
	case SENTINEL_EVENT_SYSTEM_START:
		if (sentinel->state == SENTINEL_STATE_INIT &&
		    sentinel_fault_set_is_empty(&sentinel->active_faults)) {
			sentinel->state = SENTINEL_STATE_NOMINAL;
		}
		return;
	case SENTINEL_EVENT_HEARTBEAT_TIMEOUT:
		sentinel_fault_set_add(
		    &sentinel->active_faults,
		    SENTINEL_FAULT_HEARTBEAT_LOST
		);

		if (sentinel->state == SENTINEL_STATE_NOMINAL) {
			sentinel->state = SENTINEL_STATE_DEGRADED;
		}
		return;
	case SENTINEL_EVENT_HEARTBEAT_OK:
		sentinel_clear_fault_and_recover(
		    sentinel,
		    SENTINEL_FAULT_HEARTBEAT_LOST
		);
		return;
	case SENTINEL_EVENT_VALUE_INCONSISTENT:
		sentinel_fault_set_add(
		    &sentinel->active_faults,
		    SENTINEL_FAULT_INCOHERENT_PEER_STATE
		);
		if (sentinel->state == SENTINEL_STATE_NOMINAL) {
			sentinel->state = SENTINEL_STATE_DEGRADED;
		}
		return;
	case SENTINEL_EVENT_VALUE_OK:
		sentinel_clear_fault_and_recover(
		    sentinel,
		    SENTINEL_FAULT_INCOHERENT_PEER_STATE
		);
		return;
	case SENTINEL_EVENT_COMM_LOST:
		sentinel_fault_set_add(
		    &sentinel->active_faults,
		    SENTINEL_FAULT_COMMUNICATION_LOST
		);
		sentinel->state = SENTINEL_STATE_FAIL_SAFE;
		return;
	case SENTINEL_EVENT_COMM_OK:
		sentinel_clear_fault_and_recover(
		    sentinel,
		    SENTINEL_FAULT_COMMUNICATION_LOST
		);
		return;
	case SENTINEL_EVENT_FAULT_ESCALATED:
		sentinel->state = SENTINEL_STATE_FAIL_SAFE;
		return;
	case SENTINEL_EVENT_RESET_REQUESTED:
		/*
		 * RESET_REQUESTED is accepted only from FAIL_SAFE.
		 * It is ignored in INIT, NOMINAL and DEGRADED.
		 */
		return;
	case SENTINEL_EVENT_INVALID:
	default:
		sentinel->state = SENTINEL_STATE_FAIL_SAFE;
		return;
	}
}

void sentinel_init(struct Sentinel *sentinel) {
	assert(sentinel != NULL);

	sentinel->state = SENTINEL_STATE_INIT;
	sentinel->active_faults.bits = SENTINEL_FAULT_NONE;
}

SentinelState sentinel_get_state(const struct Sentinel *sentinel) { return sentinel->state; }
