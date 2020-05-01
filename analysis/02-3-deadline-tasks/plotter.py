import matplotlib.pyplot as plt
import csv

fig, ax = plt.subplots()

f = open('./results/results.csv', 'r')
reader = csv.reader(f)

start_times = []
runtimes = []

start_time_current = 0
for line in reader:
    start_times.append(start_time_current)
    runtimes.append(int(line[3]))
    start_time_current += int(line[2])

bars = [(start, runtime) for start, runtime in zip(start_times[:100], runtimes[:100])]

ax.broken_barh(bars, (10, 9), facecolors='tab:blue')
ax.set_ylim(5, 25)
ax.set_xlabel('seconds since start')
ax.set_yticks([15, 25])
ax.set_xticks([i for i in range(0, bars[-1][1], int(1e6))])

ax.grid(True)

f.close()
plt.show()
