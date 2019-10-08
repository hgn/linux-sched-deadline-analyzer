#!/bin/sh

# start 4 processes in background
# 3 deadline scheduler processes and a busy SCHED_OTHER
# process
./src/runner ... &
PID1=$!

./src/runner ... &
PID2=$!

./src/runner ... &
PID3=$!

./src/runner ... &
PID4=$!


# execute test for 100 seconds
sleep 100

# kill everything
kill -9 $PID1
kill -9 $PID2
kill -9 $PID3
kill -9 $PID4
