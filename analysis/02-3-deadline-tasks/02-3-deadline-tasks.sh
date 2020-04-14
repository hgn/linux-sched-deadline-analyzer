#!/bin/sh

# start 4 processes in background
# 3 deadline scheduler processes and a busy SCHED_OTHER
# process
cd $(dirname $0)
sh ./configure-cpu-set.sh
echo $$ > /dev/cpuset/cpu0/tasks

../../src/runner -I 0 -o 1 --period 2000 --runtime 100 --deadline 900 --calctime 100 --sleeptime 1900 &
PID1=$!

../../src/runner -I 0 -o 1 --period 2000 --runtime 150 --deadline 500 --calctime 100 --sleeptime 1900 &
PID2=$!

../../src/runner -I 0 -o 1 --period 2000 --runtime 150 --deadline 500 --calctime 100 --sleeptime 1900 &
PID3=$!
echo $PID3 > ./deadlinerunner.pid

../../src/runner -I 0 -o 1 --cpu-iterations 10000000000 --sleeptime 0 &
PID4=$!

EVENTS='sched:sched_switch,sched:sched_wakeup'
perf record -C 0 -e $EVENTS &
PID5=$!

# execute test for N seconds
sleep 30

# kill everything
kill -15 $PID1
kill -15 $PID2
kill -15 $PID3
kill -15 $PID4
sleep 1
kill -15 $PID5

mkdir results
sleep 2
perf script > ./results/results.txt
python3 timeparser.py
