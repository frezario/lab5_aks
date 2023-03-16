import subprocess
import matplotlib.pyplot as plt
import numpy as np

outputs_min = [[], []]
outputs_avg = [[], []]
threads = np.arange(1, 17)
points = 100
repetitions = 2
func = 2
for thread in threads:
    times_1 = []
    for _ in range(repetitions):
        result = subprocess.run(["./bin/integrate_parallel", f"func{func}.cfg", str(func), str(thread)],
                                capture_output=True, text=True).stdout.split("\n")
        times_1.append(int(result[3]))

    outputs_min[0].append(min(times_1))
    outputs_avg[0].append(sum(times_1) / repetitions)

    print(thread)

for thread in threads:
    print(str(thread), str(points))
    times_2 = []
    for _ in range(repetitions):
        result = subprocess.run(["./bin/template", f"func{func}.cfg", str(func), str(thread), str(points)],
                                capture_output=True, text=True).stdout.split("\n")
        times_2.append(int(result[3]))

    print(thread)

    outputs_min[1].append(min(times_2))
    outputs_avg[1].append(sum(times_2)/repetitions)

plt.bar(threads-0.2, outputs_min[0], 0.4, label="Minimal time - no queue")
plt.bar(threads+0.2, outputs_min[1], 0.4, label="Minimal time - queue")
plt.xlabel('Threads')
plt.ylabel('Time')
plt.legend()
plt.show()

plt.bar(threads-0.2, outputs_avg[0], 0.4, label="Average time - no queue")
plt.bar(threads+0.2, outputs_avg[1], 0.4, label="Average time - queue")
plt.xlabel('Threads')
plt.ylabel('Time')
plt.legend()
plt.show()
