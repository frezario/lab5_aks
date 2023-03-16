import subprocess
import matplotlib.pyplot as plt
import numpy as np

outputs_min = [[], []]
outputs_avg = [[], []]
threads = np.arrange(1, 17)
points = !!!
repetitions = 4
for thread in threads:
    times_1 = []
    times_2 = []
    for _ in range(repetitions):
        result = subprocess.run(["./build/integrate_parallel", f"func2.cfg", 2, str(threads)],
                            capture_output=True, text=True).stdout.split("\n")
        times_1.append(int(result[3]))
        
        result = subprocess.run(["./build/integrate_parallel_queue", f"func2.cfg", 2, str(threads), str(points)],
                                capture_output=True, text=True).stdout.split("\n")
        times_2.append(int(result[3]))
        
    outputs_min[0].append(min(times_1))
    outputs_avg[0].append(sum(times_1)/repetitions)

    outputs_min[1].append(min(times_2))
    outputs_avg[1].append(sum(times_2)/repetitions)

plt.bar(threads-0.2, outputs_min[0], 0.4, labal="Minimal time - no queue")
plt.bar(threads+0.2, outmuts_min[1], 0.4, labal="Minimal time - queue")
plt.xlabel('Threads')
plt.ylabel('Time')
plt.legend()
plt.show()

plt.bar(threads-0.2, outputs_avg[0], 0.4, labal="Average time - no queue")
plt.bar(threads+0.2, outmuts_avg[1], 0.4, labal="Average time - queue")
plt.xlabel('Threads')
plt.ylabel('Time')
plt.legend()
plt.show()
