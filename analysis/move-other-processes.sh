#! /usr/bin/bash

mkdir /dev/cpuset
mount -t cgroup -o cpuset cpuset /dev/cpuset

# create a cpu-set for all conventional tasks
# assign all systems tasks to it
mkdir /dev/cpuset/sys
N=nproc
echo 1-$N > /dev/cpuset/sys/cpuset.cpus

echo 1 > /dev/cpuset/sys/cpuset.cpu_exclusive
echo 0 > /dev/cpuset/sys/cpuset.mems

# create a cpu-set exclusively for measuring the scheduler
# runner is later assigned to it
mkdir /dev/cpuset/rt
echo 0 > /dev/cpuset/rt/cpuset.cpus

echo 1 > /dev/cpuset/rt/cpuset.cpu_exclusive
echo 0 > /dev/cpuset/rt/cpuset.mems
echo 0 > /dev/cpuset/rt/cpuset.sched_load_balance
echo 1 > /dev/cpuset/rt/cpuset.mem_hardwall

ps -e -o pid |\
while read T; do
	echo $T >> /dev/cpuset/sys/tasks
done
