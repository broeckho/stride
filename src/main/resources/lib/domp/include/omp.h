#ifndef SRC_MAIN_RESOURCES_LIB_DOMP_INCLUDE_OMP_H_
#define SRC_MAIN_RESOURCES_LIB_DOMP_INCLUDE_OMP_H_

//------------------------------------------------
// https://github.com/chipbuster/libdomp
//------------------------------------------------
// THIS IS A DUMMY OPENMP HEADER FILE
// IT IS INTENDED TO BE USED WITH A
// DUMMY OPENMP LIBRARY TO DISABLE ALL OPENMP
// CONSTRUCTS.


#include<sys/time.h>
#include <cstdlib>

using omp_lock_t = int;
using omp_nest_lock_t = int;
enum omp_sched_t {omp_sched_static = 1, omp_sched_dynamic = 2, omp_sched_guided = 3, omp_sched_auto = 4};


#define NOOP ((void)0)

inline void omp_set_num_threads (int){ NOOP; }
inline int omp_get_num_threads (void){ return 1; }
inline int omp_get_max_threads (void){ return 1; }
inline int omp_get_thread_num (void){ return 0; }
inline int omp_get_num_procs (void){ return 1; }

inline int omp_in_parallel (void){ return 0; }

inline void omp_set_dynamic (int){ NOOP; }
inline int omp_get_dynamic (void){ return 0; }

inline void omp_set_nested (int){ NOOP; }
inline int omp_get_nested (void){ return 0; }

inline void omp_set_schedule(omp_sched_t, int) { NOOP; }
inline void omp_get_schedule(omp_sched_t*, int*) { NOOP; }

inline void omp_init_lock (omp_lock_t *x){ *x = 0; }
inline void omp_destroy_lock (omp_lock_t *x){ *x = -1; }
inline void omp_set_lock (omp_lock_t *x){ *x = 1; }
inline void omp_unset_lock (omp_lock_t *x){ *x = 0; }
inline int omp_test_lock (omp_lock_t *x){ return !(*x) ? *x = 1 : 0; }

inline void omp_init_nest_lock (omp_nest_lock_t *){ NOOP; }
inline void omp_destroy_nest_lock (omp_nest_lock_t *){ NOOP; }
inline void omp_set_nest_lock (omp_nest_lock_t *){ NOOP; }
inline void omp_unset_nest_lock (omp_nest_lock_t *){ NOOP; }
inline int omp_test_nest_lock (omp_nest_lock_t *){ return 1; }

inline double omp_get_wtime (void) {
    struct timeval *times = (struct timeval *) malloc(sizeof(struct timeval));
    gettimeofday(times,NULL);
    double retval = 1.0 * times->tv_sec + 0.000001 * times->tv_usec;
    return retval;
}

inline double omp_get_wtick (void){return 1.0;}

#endif
