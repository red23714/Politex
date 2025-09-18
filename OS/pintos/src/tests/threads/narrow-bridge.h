
/* File for 'narrow_bridge' task implementation.  
   SPbSTU, IBKS, 2017 */

#ifndef __narrow_bridge_h
#define __narrow_bridge_h

enum car_priority
{
   car_normal = 0,     // Low priority car
   car_emergency = 1    // High priority car
};

enum car_direction
{
   dir_left = 0,        // Going from left -> right
   dir_right = 1        // Going from right -> left
};

// !! Implement this functions:
void narrow_bridge_init(void);
void arrive_bridge(enum car_priority prio, enum car_direction dir);
void exit_bridge(enum car_priority prio, enum car_direction dir);

// Called when car going
void cross_bridge(enum car_priority prio, enum car_direction dir);

// Test entry point
void test_narrow_bridge(unsigned int num_vehicles_left, unsigned int num_vehicles_right,
        unsigned int num_emergency_left, unsigned int num_emergency_right);

#endif // __narrow_bridge_h
