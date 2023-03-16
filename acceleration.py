import subprocess
import matplotlib.pyplot as plt
import numpy as np

e_n = []
threads = range(1, 17)
points = !!!
repetitions = 4

times = []
for _ in range(repetitions):
    result = subprocess.run(["./build/integrate_parallel", f"func2.cfg", 2, 1],
                        capture_output=True, text=True).stdout.split("\n")
    times.append(int(result[3]))
L_1 = min(times)

for thread in threads:
    times.clear()
    for _ in range(repetitions):
        result = subprocess.run(["./build/integrate_parallel", f"func2.cfg", 2, str(threads)],
                            capture_output=True, text=True).stdout.split("\n")
        times.append(int(result[3]))
    e_n.append(L_1/min(times)/thread)

plt.plot(threds, e_n)
plt.xlabel('Threads')
plt.ylabel('Efficiency coefficient')
plt.show()
