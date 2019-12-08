
all:
	make -C src
	make -C analysis/01-one-sched-other-one-second/

clean:
	make -C src clean
	make -C analysis/01-one-sched-other-one-second/ clean
