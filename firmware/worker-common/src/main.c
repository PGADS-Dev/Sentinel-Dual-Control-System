#include "sentinel_state.h"
#include <stdio.h>

int main(void)
{

	SentinelState state = SENTINEL_STATE_INIT;
	printf("Actual state: %s[%d]\n", sentinel_state_to_string(state), state);

	state = SENTINEL_STATE_NOMINAL;
	printf("Actual state: %s[%d]\n", sentinel_state_to_string(state), state);

	return 0;
}
