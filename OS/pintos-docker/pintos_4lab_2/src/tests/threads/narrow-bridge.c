
/* File for 'narrow_bridge' task implementation.  
   SPbSTU, IBKS, 2017 */

#include <stdio.h>
#include "tests/threads/tests.h"
#include "threads/thread.h"
#include "threads/synch.h"
#include "narrow-bridge.h"

// Called before test. Can initialize some synchronization objects.
void narrow_bridge_init(void)
{
	// Not implemented
}

void arrive_bridge(enum car_priority prio UNUSED, enum car_direction dir UNUSED)
{
	// Not implemented. Remove UNUSED keyword if need.
}

void exit_bridge(enum car_priority prio UNUSED, enum car_direction dir UNUSED)
{
	// Not implemented. Remove UNUSED keyword if need.
}
