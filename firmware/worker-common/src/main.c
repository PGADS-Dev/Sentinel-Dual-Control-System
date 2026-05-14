#include "sentinel_state.h"
#include <stdio.h>

int main(void)
{

	SentinelState state = SENTINEL_STATE_INIT;
	printf("Actual state: %s[%d]\n", sentinel_state_to_string(state), state);

	printf("Communication Lost...\n");

	state = sentinel_next_state(state, SENTINEL_EVENT_COMMUNICATION_LOST);
	printf("Actual state: %s[%d]\n", sentinel_state_to_string(state), state);

	printf("Manual reset...\n");

	state = sentinel_next_state(state, SENTINEL_EVENT_MANUAL_RESET);
	printf("Actual state: %s[%d]\n", sentinel_state_to_string(state), state);
	return 0;
}
