/***********************************************************************
 *                   GNU Lesser General Public License
 *
 * This file is part of the GFDL Flexible Modeling System (FMS).
 *
 * FMS is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or (at
 * your option) any later version.
 *
 * FMS is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with FMS.  If not, see <http://www.gnu.org/licenses/>.
 **********************************************************************/
#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sched.h>
#include <errno.h>
#include <sys/resource.h>
#include <sys/syscall.h>

static pid_t gettid1(void)
{
#ifndef __APPLE__
  return syscall(__NR_gettid);
#endif
}

/*
 * Returns this thread's CPU affinity, if bound to a single core,
 * or else -1.
 */
int get_cpu_affinity(void)
{
#ifndef __APPLE__
  cpu_set_t coremask;		/* core affinity mask */

  CPU_ZERO(&coremask);
  if (sched_getaffinity(gettid1(),sizeof(cpu_set_t),&coremask) != 0) {
    fprintf(stderr,"Unable to get thread %d affinity. %s\n",gettid1(),strerror(errno));
  }

  int cpu;
  int first_cpu = -1;	/* first CPU in range */
  int last_cpu = -1;	/* last CPU in range */
  for (cpu=0;cpu < CPU_SETSIZE;cpu++) {
    if (CPU_ISSET(cpu,&coremask)) {
      if (first_cpu == -1) {
         first_cpu = cpu;
      } else {
        last_cpu = cpu;
      }
    }
  }

  if (last_cpu != -1) {return (first_cpu);}
  return (last_cpu == -1) ? first_cpu : -1;
#else
  return 0;
#endif
}

int get_cpu_affinity_(void) { return get_cpu_affinity(); }	/* Fortran interface */


/*
 * Set CPU affinity to one core.
 */
void set_cpu_affinity( int cpu )
{
#ifndef __APPLE__
  cpu_set_t coremask;		/* core affinity mask */

  CPU_ZERO(&coremask);
  CPU_SET(cpu,&coremask);
  if (sched_setaffinity(gettid1(),sizeof(cpu_set_t),&coremask) != 0) {
    fprintf(stderr,"Unable to set thread %d affinity. %s\n",gettid1(),strerror(errno));
  }
#endif
}

void set_cpu_affinity_(int *cpu) { set_cpu_affinity(*cpu); }	/* Fortran interface */