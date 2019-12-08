#!/bin/sh

# start 4 processes in background
# 3 deadline scheduler processes and a busy SCHED_OTHER
# process

sh ../configure-cpu-set.sh

cpu_iter=1000000

./src/runner -i cpu_iter -I 0 -s 200 -r 100 -d 150 -p 1000  &
PID1=$!

./src/runner -i cpu_iter -I 0 -s 200 -r 100 -d 150 -p 1000  &
PID2=$!

./src/runner -i cpu_iter -I 0 -s 200 -r 100 -d 150 -p 1000  &
PID3=$!

# sched other
./src/runner -i 1000000000000 -I 0 -s 200 -r 0 -d 0 -p 0  &
PID4=$!


# execute test for N seconds
sleep 120

# kill everything
kill -9 $PID1
kill -9 $PID2
kill -9 $PID3
kill -9 $PID4
