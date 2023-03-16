import numpy as np

points = np.arrange(1, 11) * 10
outputs_avg = []
for i in points:
    for _ in range(repetitions):
        result = subprocess.run(["./build/integrate_parallel_queue", f"func2.cfg", 2, 4, str(i)],
                                capture_output=True, text=True).stdout.split("\n")
        times.append(result[3])
    outputs_avg.append(sum(times) / repetitions)
plt.plot(points, outputs_avg)
plt.show()
