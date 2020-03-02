#!/bin/sh

# this test calls ../runner with no argument for sched_deadline, 
# so it will run with sched other, and it will run for a ~second
# and will sleep for a second, and so on.

# This simple non realtime run is just an minimal example to
# call perf record, followed by perf script run to analyse the
# output and print the waittime, runtime etc.
# This analyzer scripts are used by the other scripts, this is
# just an simple example to get things done.
cd $(dirname $0)
sh ../configure-super-exclusive-cpu-set.sh
../../src/runner -I 100 -o 10 --calctime 1000 --sleeptime 1000 & 
PID1=$!

# Assign runner to the measuring cpu-set
echo $PID1 > /dev/cpuset/rt/tasks

#EVENTS='{sched:sched_switch}'
EVENTS='{raw_syscalls:*,sched:sched_switch,sched:sched_process_exec,sched:sched_process_fork,sched:sched_process_exit,sched:sched_stat_runtime,sched:sched_stat_wait,sched:sched_stat_sleep,sched:sched_stat_blocked,sched:sched_stat_iowait}'

perf sched record -p $PID1 -e $EVENTS

# sleep 60
# kill -9 $PID1
