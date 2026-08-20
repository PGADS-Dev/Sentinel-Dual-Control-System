#include "sentinel_state.h"
#include "sentinel_event.h"
#include "sentinel_fault.h"

const char *sentinel_state_to_string(SentinelState state) {
	switch (state) {
	case SENTINEL_STATE_INIT:
		return "INIT";
	case SENTINEL_STATE_NOMINAL:
		return "NOMINAL";
	case SENTINEL_STATE_DEGRADED:
		return "DEGRADED";
	case SENTINEL_STATE_FAIL_SAFE:
		return "FAIL_SAFE";
	default:
		return "UNKNOWN_STATE";
	}
}

const char *sentinel_event_to_string(SentinelEvent event) {
	switch (event) {
	case SENTINEL_EVENT_INVALID:
		return "INVALID";
	case SENTINEL_EVENT_COMM_LOST:
		return "COMM_LOST";
	case SENTINEL_EVENT_COMM_OK:
		return "COMM_OK";
	case SENTINEL_EVENT_FAULT_ESCALATED:
		return "FAULT_ESCALATED";
	case SENTINEL_EVENT_HEARTBEAT_OK:
		return "HEARTBEAT_OK";
	case SENTINEL_EVENT_HEARTBEAT_TIMEOUT:
		return "HEARTBEAT_TIMEOUT";
	case SENTINEL_EVENT_RESET_REQUESTED:
		return "RESET_REQUESTED";
	case SENTINEL_EVENT_SYSTEM_START:
		return "SYSTEM_START";
	case SENTINEL_EVENT_VALUE_INCONSISTENT:
		return "VALUE_INCONSISTENT";
	case SENTINEL_EVENT_VALUE_OK:
		return "VALUE_OK";
	default:
		return "UNKNOWN_EVENT";
	}
}

const char *sentinel_fault_to_string(SentinelFault fault) {
	switch (fault) {
	case SENTINEL_FAULT_HEARTBEAT_LOST:
		return "HEARTBEAT_LOST";
	case SENTINEL_FAULT_COMMUNICATION_LOST:
		return "COMMUNICATION_LOST";
	case SENTINEL_FAULT_INCOHERENT_PEER_STATE:
		return "INCOHERENT_PEER_STATE";
	default:
		return "UNKNOWN_FAULT";
	}
}

SentinelState sentinel_next_state(SentinelState current, SentinelEvent event) {
	switch (current) {
	case SENTINEL_STATE_INIT:
		if (event == SENTINEL_EVENT_SYSTEM_START) {
			return SENTINEL_STATE_NOMINAL;
		}
		return SENTINEL_STATE_INIT;
	case SENTINEL_STATE_NOMINAL:
		if (event == SENTINEL_EVENT_HEARTBEAT_TIMEOUT || event == SENTINEL_EVENT_VALUE_INCONSISTENT ||
		    event == SENTINEL_EVENT_COMM_LOST) {
			return SENTINEL_STATE_DEGRADED;
		}
		return SENTINEL_STATE_NOMINAL;
	case SENTINEL_STATE_DEGRADED:
		if (event == SENTINEL_EVENT_HEARTBEAT_TIMEOUT || event == SENTINEL_EVENT_VALUE_INCONSISTENT ||
		    event == SENTINEL_EVENT_COMM_LOST) {
			return SENTINEL_STATE_FAIL_SAFE;
		}
		return SENTINEL_STATE_DEGRADED;
	case SENTINEL_STATE_FAIL_SAFE:
		if (event == SENTINEL_EVENT_RESET_REQUESTED) {
			return SENTINEL_STATE_INIT;
		}
		return SENTINEL_STATE_FAIL_SAFE;
	default:
		return SENTINEL_STATE_FAIL_SAFE;
	}
}
