import re


def grep_time(line):
    match = re.match(r'(\d+\.\d+)', line)
    f = float(match.group(0))
    i = int(f * 1000)
    return i


def line_parser(line, switch_in, switch_out):
    if "next_comm=runner" in line:
        switch_in.append(grep_time(line))
    elif "prev_comm=runner" in line:
        switch_out.append(grep_time(line))


def main():
    rein = open("rein.txt")

    switch_in  = []
    switch_out = []

    for line in rein:
        line_parser(line, switch_in, switch_out)

    
