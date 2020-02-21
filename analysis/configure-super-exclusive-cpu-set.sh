#! /usr/bin/bash

if [ ! -d "$/dev/cpuset/" ]; then
	mkdir /dev/cpuset
	mount -t cgroup -o cpuset cpuset /dev/cpuset

	mkdir /dev/cpuset/sys
	echo 1-7 > /dev/cpuset/sys/cpuset.cpus

	echo 1 > /dev/cpuset/sys/cpuset.cpu_exclusive
	echo 0 > /dev/cpuset/sys/cpuset.mems

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
fi
