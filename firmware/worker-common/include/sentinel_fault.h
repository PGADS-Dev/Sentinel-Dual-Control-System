#ifndef SENTINEL_FAULT_H
#define SENTINEL_FAULT_H

#include <stdint.h>

typedef enum {
	SENTINEL_FAULT_NONE = 0u,
	SENTINEL_FAULT_HEARTBEAT_LOST = 1u << 0,
	SENTINEL_FAULT_INCOHERENT_PEER_STATE = 1u << 1,
	SENTINEL_FAULT_COMMUNICATION_LOST = 1u << 2
} SentinelFault;

typedef struct {
	uint32_t bits;
} SentinelFaultSet;

const char *sentinel_fault_to_string(SentinelFault fault);

#endif // !SENTINEL_FAULT_H
