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
#define S_TO_US_FACTOR (1ULL * 1000 * 1000)

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
	unsigned sleeptime_ms;
};


int sched_setattr(pid_t pid,
               const struct sched_attr *attr,
               unsigned int flags)
{
	printf("Periode: %llu, Runtime: %llu, Deadline: %llu\n",
			attr->sched_period, attr->sched_runtime, attr->sched_deadline);

	return syscall(__NR_sched_setattr, pid, attr, flags);
}


int sched_getattr(pid_t pid,
               struct sched_attr *attr,
               unsigned int size,
               unsigned int flags)
{
	return syscall(__NR_sched_getattr, pid, attr, size, flags);
}


void print_help(void)
{
	printf("runner [options]\tAll times in milliseconds\n
			-i <number> number of CPU iterations\n\
			-I <number> number of program loop iterations. Used to prevent\n\t\
			the program running forever and potentially freezing your system.\n\t\
			set to 0 to run forever.\n\
			-s <number> sleep-time for xsleep\n\
			-p <number> period for sched_dead\n\
			-r <number> run-time for sched_dead\n\
			-d <number> deadline for sched_dead\n");
}


void parse_args(struct config *cfg, int argc, char *argv[])
{
	int opt;
	while ((opt = getopt(argc, argv, "i:I:s:r:p:d:")) != -1) {
		switch (opt) {
		case 'i':
			cfg->cpu_iterations = (unsigned long long)atoll(optarg);
			break;
		case 'I':
			cfg->program_iterations = (unsigned)atoi(optarg);
			break;
		case 's':
			cfg->sleeptime_ms = (unsigned)atoi(optarg);
			printf("sleeptime konfiguriert: %u\n", cfg->sleeptime_ms);
			break;
		/* scheduler times are ns, user gives ms */
		case 'r':
			cfg->attr.sched_runtime = MS_TO_NS_FACTOR * atoi(optarg);
			break;
		case 'd':
			cfg->attr.sched_deadline = MS_TO_NS_FACTOR * atoi(optarg);
			break;
		case 'p':
			cfg->attr.sched_period = MS_TO_NS_FACTOR * atoi(optarg);
			break;
		default:
			fprintf(stderr, "arguments wrong somehow, exiting...\n");
			print_help();
			exit(EXIT_FAILURE);
			break;
		}
	}
}


/* currently unused. setaffinity does not work with sched_dead */
void configure_cpu()
{
	cpu_set_t set;
	CPU_ZERO(&set);
	CPU_SET(0, &set);
	if (sched_setaffinity(0, sizeof(set), &set) == -1) {
		perror("configure_cpu");
		exit(EXIT_FAILURE);
	}
	puts("Successfully set to cpu 0.");
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
	unsigned diff=0, sudiff=0;
	time_t start, end;
	suseconds_t sustart, suend;
	start = tv_start.tv_sec;
	end = tv_end.tv_sec;
	sustart = tv_start.tv_usec;
	suend = tv_end.tv_usec;

	/*
	 printf("ende: %li\n", end); 
	 printf("start: %li\n", start); 
	 printf("suende: %li\n", suend); 
	 printf("sustart: %li\n", sustart); 
	*/

	diff = end - start;
	/* printf("End - Start in s: %u\n", diff); */
	sudiff = suend - sustart;
	/* printf("Rest End - Start in us: %u\n", sudiff); */

	return diff * S_TO_US_FACTOR + sudiff;
}


void busy_cycles(struct config *cfg)
{
	unsigned long long i = cfg->cpu_iterations;
	struct timeval tv_start, tv_end;

	gettimeofday(&tv_start, NULL);

	while (i--);

	gettimeofday(&tv_end, NULL);
	printf("%u us run-time\n", us_timediff(tv_start, tv_end));
}


void xsleep(struct config *cfg)
{
	struct timeval tv_start, tv_end;
	gettimeofday(&tv_start, NULL);

	/* printf("Sleeping %u us...\n", cfg->sleeptime_ms); */
	usleep(cfg->sleeptime_ms * 1000);

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
		.cpu_iterations = 1 * 1000 * 1000,
		.program_iterations = 1,
		.sleeptime_ms = 1000,
	};

	parse_args(&cfg, argc, argv);

	if (attr.sched_runtime > 0 && attr.sched_period > 0 && attr.sched_deadline > 0)
		init_sched_dead(&cfg);

	for (i=0; run; i++) {
		busy_cycles(&cfg);
		xsleep(&cfg);

		/* run forever if program_iterations is set to 0 */
		if (cfg.program_iterations != 0 && i == cfg.program_iterations)
			run = false;
	}

	return EXIT_SUCCESS;
}
