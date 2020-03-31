import re

first_time = True

def grep_time(line):
    match = re.search(r'(\d+\.\d+)', line)
    f = float(match.group(0))
    i = int(f * 1000 * 1000)
    return i


def line_parser(line, switch_in, switch_out):
    global first_time

    if "sched_switch" in line:
        if "=> runner" in line:
            switch_in.append(grep_time(line))
        elif ": runner" in line and not "=> runner" in line:
            switch_out.append(grep_time(line))

        # Make sure the first match is a switch_in
        if first_time:
            if len(switch_out) > len(switch_in):
                switch_out.pop()
            first_time = False


def main():
    rein = open("rein.txt")

    switch_in  = []
    switch_out = []

    for line in rein:
        line_parser(line, switch_in, switch_out)
    rein.close()

    print('switch_in  switch_out  delta   (in microseconds)')
    for hinein, hinaus in zip(switch_in, switch_out):
        delta = hinaus - hinein
        print(hinein, hinaus, delta)


if __name__ == '__main__':
    main()
