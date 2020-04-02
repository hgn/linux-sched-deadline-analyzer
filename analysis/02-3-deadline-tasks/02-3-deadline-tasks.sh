#!/bin/sh

# start 4 processes in background
# 3 deadline scheduler processes and a busy SCHED_OTHER
# process
cd $(dirname $0)
sh ../configure-super-exclusive-cpu-set.sh

perf sched record ../../src/runner -I 0 --period 2000 --runtime 150 --deadline 500 --calctime 100 --sleeptime 1900 &
PID1=$!
echo $PID1

../../src/runner -I 0 --period 2000 --runtime 150 --deadline 500 --calctime 100 --sleeptime 1900 &
PID2=$!

../../src/runner -I 0 --period 2000 --runtime 150 --deadline 500 --calctime 100 --sleeptime 1900 &
PID3=$!

# sched other. With this config it will attempt to calculate forever
../../src/runner -I 0 --cpu-iterations 10000000000 --sleeptime 0 &
PID4=$!

# execute test for N seconds
sleep 60

EVENTS='sched:sched_switch,sched:sched_wakeup'
perf record -C 3 -e $EVENTS &
PID5=$!

# kill everything
kill -9 $PID1
kill -9 $PID2
kill -9 $PID3
kill -9 $PID4
kill -9 $PID5

mkdir results
sleep 2
echo $(PID4) > ./deadlinerunner.pid
