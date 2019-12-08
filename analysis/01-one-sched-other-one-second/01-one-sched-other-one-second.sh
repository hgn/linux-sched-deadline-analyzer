#!/bin/sh

# this test calls ../runner with no argument for sched_deadline, 
# so it will run with sched other, and it will run for a ~second
# and will sleep for a second, and so on.

# This simple non realine run is just an ninimal example to
# call perf record, followed by perf script run to analyse the
# output and print the waittime, runtime etc.
# This analyzer scripts are used by the other scripts, this is
# just an simple example to get things done.

sh ../configure-cpu-set.sh
../../src/runner -i 1000000 -I 0 -s 100 -r 100 -d 200 -p 1000 &
PID1=$!

perf record -p $PID1
PID_PERF=$!

sleep 10

kill $PID1
kill $PID_PERF
