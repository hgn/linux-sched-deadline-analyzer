#!/bin/sh

# this test calls ../runner with no argument for sched_deadline, 
# so it will run with sched other, and it will run for a ~second
# and will sleep for a second, and so on.

# This simple non realine run is just an ninimal example to
# call perf record, followed by perf script run to analyse the
# output and print the waittime, runtime etc.
# This analyzer scripts are used by the other scripts, this is
# just an simple example to get things done.
cd $(dirname $0)
sh ../configure-cpu-set.sh
../../src/runner -I 0 --period 2000 --runtime 150 --deadline 500 --calctime 100 --sleeptime 2000 & 
PID1=$!

EVENTS='{raw_syscalls:*,sched:sched_switch,sched:sched_migrate_task,'
'sched:sched_process_exec,sched:sched_process_fork,sched:sched_process_exit,'
'sched:sched_stat_runtime,sched:sched_stat_wait,sched:sched_stat_sleep,'
'sched:sched_stat_blocked,sched:sched_stat_iowait}'

perf sched record -p $PID1 -e $EVENTS &

sleep 60
kill -9 $PID1
