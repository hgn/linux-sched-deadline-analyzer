#!/bin/sh

# start 4 processes in background
# 3 deadline scheduler processes and a busy SCHED_OTHER
# process
cd $(dirname $0)
sh ../configure-cpu-set.sh


perf sched record ../../src/runner -I 0 --period 2000 --runtime 150 --deadline 500 --calctime 100 --sleeptime 2000 & 
PID1=$!
echo $PID1

../../src/runner -I 0 --period 2000 --runtime 150 --deadline 500 --calctime 100 --sleeptime 2000 & 
PID2=$!

../../src/runner -I 0 --period 2000 --runtime 150 --deadline 500 --calctime 100 --sleeptime 2000 & 
PID3=$!

# sched other
../../src/runner -I 0 --cpu-iterations 1000000000000 --sleeptime 0 & 
PID4=$!


# execute test for N seconds
sleep 60

# kill everything
kill -9 $PID1
kill -9 $PID2
kill -9 $PID3
kill -9 $PID4
