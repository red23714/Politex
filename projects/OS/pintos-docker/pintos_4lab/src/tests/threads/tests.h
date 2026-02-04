#ifndef TESTS_THREADS_TESTS_H
#define TESTS_THREADS_TESTS_H

void run_test (const char *);

typedef void test_func (void);
typedef void test_func_args_1uint (unsigned int);
typedef void test_func_args_2uint (unsigned int, unsigned int);
typedef void test_func_args_3uint (unsigned int, unsigned int, unsigned int);
typedef void test_func_args_4uint (unsigned int, unsigned int, unsigned int, unsigned int);

extern test_func test_alarm_single;
extern test_func test_alarm_multiple;
extern test_func test_alarm_simultaneous;
extern test_func test_alarm_priority;
extern test_func test_alarm_zero;
extern test_func test_alarm_negative;
extern test_func test_priority_change;
extern test_func test_priority_donate_one;
extern test_func test_priority_donate_multiple;
extern test_func test_priority_donate_multiple2;
extern test_func test_priority_donate_sema;
extern test_func test_priority_donate_nest;
extern test_func test_priority_donate_lower;
extern test_func test_priority_donate_chain;
extern test_func test_priority_fifo;
extern test_func test_priority_preempt;
extern test_func test_priority_sema;
extern test_func test_priority_condvar;
extern test_func test_mlfqs_load_1;
extern test_func test_mlfqs_load_60;
extern test_func test_mlfqs_load_avg;
extern test_func test_mlfqs_recent_1;
extern test_func test_mlfqs_fair_2;
extern test_func test_mlfqs_fair_20;
extern test_func test_mlfqs_nice_2;
extern test_func test_mlfqs_nice_10;
extern test_func test_mlfqs_block;
extern test_func test_max_threads;
extern test_func test_max_rec_calls;
extern test_func test_max_mem_malloc;
extern test_func test_max_mem_calloc;
extern test_func test_max_mem_palloc;
extern test_func test_ticks_stats;
extern test_func test_threads_audit;
extern test_func test_threads_term;
extern test_func test_threads_pause_resume;
extern test_func test_new_alg;

void test_narrow_bridge(unsigned int num_vehicles_left, unsigned int num_vehicles_right,
        unsigned int num_emergency_left, unsigned int num_emergency_right);

void test_hair(unsigned int num_clients, unsigned int interval);
void test_bear(unsigned int num_bees, unsigned int pot_size);
void test_birds(unsigned int num_chicks, unsigned int dish_size);
void test_rolcoast(unsigned int num_clients, unsigned int carriage_size, unsigned int interval);
void test_shower(unsigned int num_men, unsigned int num_women, unsigned int interval);
void test_molec(unsigned int num_oxygen, unsigned int num_hydrogen, int iterval);
void test_smokers(void);


void msg (const char *, ...);
void fail (const char *, ...);
void pass (void);

#endif /* tests/threads/tests.h */

