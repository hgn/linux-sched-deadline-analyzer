all:
	make -C src

analyze: all
	make -C analysis/01-one-sched-other-one-second/

analyze-deadline: all
	make -C analysis/02-3-deadline-tasks

clean:
	make -C src clean
	make -C analysis/01-one-sched-other-one-second/ clean
