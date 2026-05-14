#pragma once

typedef enum {
	SENTINEL_FAULT_HEARTBEAT_LOST,
	SENTINEL_FAULT_COMMUNICATION_LOST,
	SENTINEL_FAULT_INCOHERENT_PEER_STATE
} SentinelFault;

const char *sentinel_fault_to_string(SentinelFault fault);
