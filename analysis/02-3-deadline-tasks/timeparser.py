import re
import csv

first_time = True

def grep_time(line):
    match = re.search(r'(\d+\.\d+)', line)
    f = float(match.group(0))
    i = int(f * 1000 * 1000)
    return i


def switches_from_runner(line):
    match = re.search(r'=runner.*?==>', line)

    if match is None:
        return False
    else:
        return True


def switches_to_runner(line):
    match = re.search(r'==>.*?runner', line)

    if match is None:
        return False
    else:
        return True


def line_parser(line, switch_in, switch_out):
    global first_time

    if "sched_switch" in line:
        if switches_to_runner(line):
            switch_in.append(grep_time(line))
        elif switches_from_runner(line):
            switch_out.append(grep_time(line))

        # Make sure the first match is a switch_in
        if first_time:
            if len(switch_out) > len(switch_in):
                switch_out.pop()
            first_time = False


def storer(switch_in, switch_out):
    f = open('./results/results.csv', 'wt')
    first_time = True
    t_in_prev = 0
    print("Times in microseconds.")
    print("{:>5} {:>20} {:>30} {:>20}"
            .format("Nr.", "switch_in_timestamp", "time_since_list_switch_in",
                "runtime"))
    writer = csv.writer(f, delimiter=',')

    iter_list = zip(range(1, len(switch_in) + 1), switch_in, switch_out)
    for nr, t_in, t_out in iter_list:
        since_last = t_in - t_in_prev
        runtime = t_out - t_in

        if first_time:
            since_last = 0
            first_time = False

        line_new = '{:>5} {:>20} {:>30} {:>20}'.format(nr, t_in, since_last, runtime)
        print(line_new)
        t_in_prev = t_in
        if nr % 5 == 0:
            print("")
        writer.writerow([nr, t_in, since_last, runtime])

    f.close()



def main():
    f = open("./results/results.txt")
    pid = 0
    with open("./deadlinerunner.pid", "r") as pidfile:
        pid = str(pidfile.readline()).rstrip('\n')

    pid_alone = " " + pid + " "
    pid_from_other_proc = "pid=" + pid
    print(pid)

    switch_in  = []
    switch_out = []

    for line in f:
        if pid_alone in line or pid_from_other_proc in line:
            print(line)
            line_parser(line, switch_in, switch_out)
    f.close()

    storer(switch_in, switch_out)


if __name__ == '__main__':
    main()
