#include <stdio.h>
#include <stdlib.h>

struct args {
	unsigned cpu_iterations;
	unsigned sleep_time_ms;

	unsigned deadline_runtime;
	unsigned deadline_period;
	// unsigned is not correct, simple
	// use all original values for setting
	// the deadline scheduler via setsched...()
};

void parse_args(struct args *args)
{
	if (0) {
		fprintf(stderr, "arguments wrong somehow, exiting");
		exit(EXIT_FAILURE);
	}
}

void init_program(struct args *args)
{
	// if uid != 0, print error and help screen and
	// exit now (we need root for this shit)

	// bind program to cpu 0

	// if args.deadline, set scheduler policy here or
	// in a seperate function
}

void busy_cycles(&args)
{
	int i = args->cpu_iterations;

	// pseudocode
	gettimeofday(&start)

	while (i--)
		;

	// pseudocode
	gettimeofday(&end);

	print("%u us runtime time", end - start);
}

void xsleep(struct args *args)
{
	// pseudocode
	gettimeofday(&start)

	usleep(args->sleep_time_ms);

	// pseudocode
	gettimeofday(&end);

	print("%u us sleep time", end - start);
}

int main(int argc, char **argv)
{
	int ret;
	struct args args;

	parse_args(&args, argc, argv);
	init_program(&args);

	while (1) {
		busy_cycles(&args);
		xsleep(&args);
	}
}
