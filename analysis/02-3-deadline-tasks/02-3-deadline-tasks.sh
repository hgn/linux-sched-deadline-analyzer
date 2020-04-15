#!/bin/sh

# start 4 processes in background
# 3 deadline scheduler processes and a busy SCHED_OTHER
# process
cd $(dirname $0)
sh ./configure-cpu-set.sh
echo $$ > /dev/cpuset/cpu0/tasks

../../src/runner -I 0 -o 5 --period 1000 --runtime 100 --deadline 200 --cpu-iterations 100000000000 --sleeptime 0 &
PID1=$!
echo $PID1 > ./deadlinerunner1.pid

../../src/runner -I 0 -o 0 --period 1000 --runtime 100 --deadline 200 --sleeptime 0 &
PID2=$!
echo $PID2 > ./deadlinerunner2.pid

../../src/runner -I 0 -o 0 --period 1000 --runtime 100 --deadline 200 --sleeptime 0 &
PID3=$!
echo $PID3 > ./deadlinerunner3.pid

../../src/runner -I 0 -o 0 --cpu-iterations 100000000000 --sleeptime 0 &
PID4=$!
echo $PID4 > ./sched_other_runner1.pid

EVENTS='sched:sched_switch,sched:sched_wakeup'
perf record -C 0 -e $EVENTS &
PID5=$!

# execute test for N seconds
sleep 120

# kill everything
kill -15 $PID1
#kill -15 $PID2
#kill -15 $PID3
#kill -15 $PID4
sleep 1
kill -15 $PID5

mkdir results
sleep 2 # We need to wait a bit, otherwise perf script will find an empty file
perf script > ./results/results.txt
