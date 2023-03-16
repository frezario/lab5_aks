import numpy as np
import subprocess
import matplotlib.pyplot as plt

points = np.arange(1, 5) * 100
outputs_avg = []
repetitions = 2
for i in points:
    times = []
    for _ in range(repetitions):
        result = subprocess.run(["./bin/template", f"func1.cfg", "1", "4", str(i)],
                                capture_output=True, text=True).stdout.split("\n")
        times.append(int(result[3]))
    print(i)
    outputs_avg.append(sum(times) / repetitions)
plt.plot(points, outputs_avg)
plt.xlabel('Points')
plt.ylabel('Time')
plt.show()
