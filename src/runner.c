#define _GNU_SOURCE
#include <sched.h>
#include <getopt.h>
#include <stdbool.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <linux/unistd.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <sys/syscall.h>
#include <pthread.h>

#define gettid() syscall(__NR_gettid)

#define SCHED_DEADLINE       6

#define MS_TO_NS_FACTOR (1ULL * 1000 * 1000)
#define MS_TO_US_FACTOR (1ULL * 1000)
#define S_TO_US_FACTOR (1ULL * 1000 * 1000)

#define CALC_TIME_US (1ULL * 100 * 1000)

#define DEFAULT_SLEEPTIME_MS 1000
#define DEFAULT_CALCTIME_MS 1000

#define OAKING_LOOPS 10

/* XXX use the proper syscall numbers */
#ifdef __x86_64__
#define __NR_sched_setattr           314
#define __NR_sched_getattr           315
#endif

#ifdef __i386__
#define __NR_sched_setattr           351
#define __NR_sched_getattr           352
#endif

#ifdef __arm__
#define __NR_sched_setattr           380
#define __NR_sched_getattr           381
#endif

struct sched_attr {
     __u32 size;

     __u32 sched_policy;
     __u64 sched_flags;

     /* SCHED_NORMAL, SCHED_BATCH */
     __s32 sched_nice;

     /* SCHED_FIFO, SCHED_RR */
     __u32 sched_priority;

     /* SCHED_DEADLINE (nsec) */
     __u64 sched_runtime;
     __u64 sched_deadline;
     __u64 sched_period;
};


struct config {
	struct sched_attr attr;
	unsigned long long cpu_iterations, program_iterations;
	unsigned sleeptime_ms, oak_runs;
	unsigned long long calc_time_us;
};


int sched_setattr(pid_t pid, const struct sched_attr *attr, unsigned int flags)
{
	printf("Set: Period: %llu us, Runtime: %llu us, Deadline: %llu\n us",
			attr->sched_period, attr->sched_runtime, attr->sched_deadline);

	return syscall(__NR_sched_setattr, pid, attr, flags);
}


int sched_getattr(pid_t pid, struct sched_attr *attr, unsigned int size,
		unsigned int flags)
{
	return syscall(__NR_sched_getattr, pid, attr, size, flags);
}


void print_help(void)
{
	printf("runner [options]\t(All time constants in milliseconds)\n"
			"\t-i\n\t--cpu-iterations <number> number of CPU iterations.\n"
			"\t\tTypically you don't want to set this number manually.\n\n"
			"\t-I\n\t--program-iterations <number> number of program loop"
			" iterations.\n\t\tUsed to prevent "
			"the program running forever and potentially freezing\n\t\tyour system."
			" Set to 0 to run forever.\n\n"
			"\t-s\n\t--sleeptime <number> sleep-time for xsleep\n\n"
			"\t-c\n\t--calctime <number> time the program should calculate"
			" uninterrupted.\n\n"
			"\t-p\n\t--period <number> period for sched_dead\n\n"
			"\t-r\n\t--runtime <number> run-time for sched_dead\n\n"
			"\t-d\n\t--deadline <number> deadline for sched_dead\n\n"
			"\t-z\n\t--set-cpu Try to set the cpu-affinity to core0."
			"Typically not used when testing with CPU-Sets\n\n"
			"\t-o\n\t--oak-iterations <number> NO. of iterations to oak"
			" cpu iterations to fit configured calctime.\n\n");
}


void configure_cpu()
{
	const pid_t this_thread = 0;
	cpu_set_t set;
	CPU_ZERO(&set);
	CPU_SET(0, &set);

	if (sched_setaffinity(this_thread, sizeof(set), &set) == -1) {
		perror("configure_cpu");
		exit(EXIT_FAILURE);
	}
	puts("Successfully set to cpu 0.");
}


void parse_args(struct config *cfg, int argc, char **argv)
{
	int opt, option_index = 0;
	enum longopt_nr {
		CPU_ITERATIONS = 1,
		PROGRAM_ITERATIONS,
		SLEEPTIME,
		RUNTIME,
		CALCTIME,
		DEADLINE,
		PERIOD,
		OAK_RUNS,
		CPU,
		HELP,
	};

	static struct option long_options[] = {
	   {"cpu-iterations", required_argument, NULL, CPU_ITERATIONS},
	   {"program-iterations", required_argument, NULL, PROGRAM_ITERATIONS},
	   {"sleeptime",  required_argument, NULL, SLEEPTIME},
	   {"runtime", required_argument, NULL, RUNTIME},
	   {"calctime", required_argument, NULL, CALCTIME},
	   {"deadline", required_argument, NULL, DEADLINE},
	   {"period", required_argument, NULL, PERIOD},
	   {"oak-iterations", required_argument, NULL, OAK_RUNS},
	   {"set-cpu", no_argument, NULL, CPU},
	   {"help", no_argument, NULL, HELP},
	   {NULL, no_argument, NULL, 0}
	};

	while ((opt = getopt_long(argc, argv, "c:i:I:s:r:p:d:hzo:",
				long_options, &option_index)) != -1) {
		switch (opt) {
		case CPU_ITERATIONS:
		case 'i':
			cfg->cpu_iterations = (unsigned long long)atoll(optarg);
			break;
		case PROGRAM_ITERATIONS:
		case 'I':
			cfg->program_iterations = (unsigned)atoi(optarg);
			break;
		case SLEEPTIME:
		case 's':
			cfg->sleeptime_ms = (unsigned)atoi(optarg);
			break;
		/* scheduler times are ns, user gives ms */
		case RUNTIME:
		case 'r':
			cfg->attr.sched_runtime = (__u64)(MS_TO_NS_FACTOR * atoi(optarg));
			break;
		case CALCTIME:
		case 'c':
			cfg->calc_time_us =
				(unsigned long long)(MS_TO_US_FACTOR * atoll(optarg));
			break;
		case DEADLINE:
		case 'd':
			cfg->attr.sched_deadline = (__u64)(MS_TO_NS_FACTOR * atoi(optarg));
			break;
		case PERIOD:
		case 'p':
			cfg->attr.sched_period = (__u64)(MS_TO_NS_FACTOR * atoi(optarg));
			break;
		case OAK_RUNS:
		case 'o':
			cfg->oak_runs = (unsigned)atoi(optarg);
			break;
		case CPU:
		case 'z':
			configure_cpu();
			break;
		case HELP:
		case 'h':
			print_help();
			exit(EXIT_SUCCESS);
			break;
		default:
			fprintf(stderr, "arguments wrong somehow, exiting...\n");
			print_help();
			exit(EXIT_FAILURE);
			break;
		}
	}
}


void init_sched_dead(struct config *cfg)
{
	int ret = sched_setattr(0, &(cfg->attr), 0);
	if (ret < 0) {
		perror("sched_setattr");
		exit(EXIT_FAILURE);
	}
}


unsigned us_timediff(struct timeval tv_start, struct timeval tv_end)
{
	unsigned diff = 0, sudiff = 0;
	time_t start, end;
	suseconds_t sustart, suend;
	start = tv_start.tv_sec;
	end = tv_end.tv_sec;
	sustart = tv_start.tv_usec;
	suend = tv_end.tv_usec;

	diff = end - start;
	/* printf("End - Start in s: %u\n", diff); */
	sudiff = suend - sustart;
	/* printf("Rest End - Start in us: %u\n", sudiff); */

	return diff * S_TO_US_FACTOR + sudiff;
}


unsigned busy_cycles(unsigned long long iterations)
{
	struct timeval tv_start, tv_end;
	static unsigned cycles = 0;
	printf("Busy cycle: %u\n", cycles++);

	gettimeofday(&tv_start, NULL);
	// printf("start iterating. Iterations: %llu\n", iterations);
	while (iterations--);
	// puts("end iterating.");
	gettimeofday(&tv_end, NULL);
	return us_timediff(tv_start, tv_end);
}


static inline bool five_perct_exact(unsigned now, unsigned goal)
{
	if ((now > goal * 0.95) && (now < goal * 1.05))
		return true;
	else
		return false;
}


void oak_cpu(struct config *cfg)
{
	long long calctime_now = 0, calctime_goal = cfg->calc_time_us;
	unsigned long long averaging = 0;
	long long reg = 0, i;

	puts("entering oak loop");
	for (i = 0; i < OAKING_LOOPS; i++) {
		calctime_now = 0, reg = 0;
		reg = calctime_goal * 1000;

		do {
			calctime_now = busy_cycles(reg);
			if (calctime_now > calctime_goal)
				reg -= reg / 2;
			else
				reg += reg / 2;

		} while (!five_perct_exact(calctime_now, calctime_goal));
		printf("Calctime now: %llu, goal: %llu\n",
				calctime_now, calctime_goal);

		averaging += reg;
	}

	cfg->cpu_iterations = averaging / OAKING_LOOPS;
	printf("Set CPU-iterations to %llu.\n", cfg->cpu_iterations);
	puts("Done oaking.");
}


void xsleep(struct config *cfg)
{
	if (cfg->sleeptime_ms == 0)
		return;

	struct timeval tv_start, tv_end;
	gettimeofday(&tv_start, NULL);
	usleep(cfg->sleeptime_ms * MS_TO_US_FACTOR);
	gettimeofday(&tv_end, NULL);

	printf("%u us sleep time\n", us_timediff(tv_start, tv_end));
}


int main(int argc, char *argv[])
{
	unsigned i;
	bool run = true;

	struct sched_attr attr = {
		.size = sizeof(struct sched_attr),
		.sched_flags = 0,
		.sched_nice = 0,
		.sched_priority = 0,
		.sched_policy = SCHED_DEADLINE,
		/* init to 0, to detect if sched_dead is on or off */
		.sched_runtime  = 0, 
		.sched_period   = 0,
		.sched_deadline = 0,
	};

	struct config cfg = {
		.attr = attr,
		.cpu_iterations = 1 * 1000,
		.program_iterations = 1,
		.oak_runs = 1,
		.sleeptime_ms = DEFAULT_SLEEPTIME_MS,
		.calc_time_us = DEFAULT_CALCTIME_MS * MS_TO_US_FACTOR,
	};

	parse_args(&cfg, argc, argv);
	oak_cpu(&cfg);

	if (cfg.attr.sched_runtime > 0 && cfg.attr.sched_period > 0 &&
			cfg.attr.sched_deadline > 0) {
		init_sched_dead(&cfg);
	}

	for (i = 0; run; i++) {
		printf("Calculated for %u us.\n", busy_cycles(cfg.cpu_iterations));
		xsleep(&cfg);

		/* run forever if program_iterations is set to 0 */
		if (cfg.program_iterations != 0 && i == cfg.program_iterations)
			run = false;
	}

	return EXIT_SUCCESS;
}
