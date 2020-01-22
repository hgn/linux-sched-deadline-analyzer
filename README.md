# Linux Deadline Scheduler Analyzer and Testing Tool

## Build Perf
Unfortunately, this repo uses the perf script python interpreter – which apparently
is not supported anymore on many default package systems (debian).
Therefore, you have to build the userspace counterpart of perf yourself from
the kernel sources.
To do so, do the following:

1. Clone `/tools/perf` from the linux kernel. 
2. On Debian, install
* flex
* bison
* libdw-dev
* systemtap-sdt-dev
* libelf-dev
3. In `/perf`, run `make`
4. Make sure there's a green `on` at `libpython`

## Additional Information

https://github.com/hgn/linux-rt-deadline-analyzer/blob/master/workers/on-off.c

## How to Start

## Analyzer Zoo

What analzer tools are used and how

```
perf sched record -- sleep 1
perf sched latency
```

Should be a good start.


## Results


