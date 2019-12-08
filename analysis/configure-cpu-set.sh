#/bin/sh

mkdir /dev/cpuset
mount -t cgroup -o cpuset cpuset /dev/cpuset
mkdir /dev/cpuset/cpu0
echo 0 > /dev/cpuset/cpu0/cpuset.cpus
echo 0 > /dev/cpuset/cpu0/cpuset.mems
echo 1 > /dev/cpuset/cpuset.cpu_exclusive
echo 0 > /dev/cpuset/cpuset.sched_load_balance
echo 1 > /dev/cpuset/cpu0/cpuset.cpu_exclusive
echo 1 > /dev/cpuset/cpu0/cpuset.mem_exclusive
echo $$ > /dev/cpuset/cpu0/tasks
