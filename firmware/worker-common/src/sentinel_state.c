#include "sentinel_state.h"
#include "sentinel_fault.h"

const char* sentinel_state_to_string(SentinelState state) {
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

const char* sentinel_event_to_string(SentinelEvent event) {
	switch (event) {
	case SENTINEL_EVENT_INVALID:
		return "INVALID";
	case SENTINEL_EVENT_STARTUP_COMPLETE:
		return "STARTUP_COMPLETE";
	case SENTINEL_EVENT_HEARTBEAT_WARNING:
		return "HEARTBEAT_WARNING";
	case SENTINEL_EVENT_COMMUNICATION_LOST:
		return "COMMUNICATION_LOST";
	case SENTINEL_EVENT_INCOHERENT_PEER_STATE:
		return "INCOHERENT_PEER_STATE";
	case SENTINEL_EVENT_FAULT_CLEARED:
		return "FAULT_CLEARED";
	case SENTINEL_EVENT_FAULT_ESCALATED:
		return "FAULT_ESCALATED";
	case SENTINEL_EVENT_MANUAL_RESET:
		return "MANUAL_RESET";
	default:
		return "UNKNOWN_EVENT";
	}
}

const char* sentinel_fault_to_string(SentinelFault fault) {
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
