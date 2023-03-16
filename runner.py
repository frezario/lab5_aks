import subprocess
import sys
import scipy
from itertools import combinations


def run_executable(func, threads, points):
    cmd = ["./build/integrate_parallel_queue", f"func{func}.cfg", str(func), str(threads), str(points)]
    result = subprocess.run(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    stdout = result.stdout.decode("utf-8")
    exit_code = result.returncode
    if exit_code != 0:
        print(f"Error in {func} func:", result.stderr.decode("utf-8"))
        exit(exit_code)
    else:
        return stdout


def parse_abs(func):
    with open(f"func{func}.cfg", 'r') as f:
        for line in f:
            line = "".join(line.split("#")[0].split(" ")).split("=")
            if line[0] == 'abs_err':
                return float(line[1])


def analyze(arguments):
    repetitions, threads, points = arguments
    results = []
    for func in range(1, 4):
        integrals, times = [0] * repetitions, [0] * repetitions
        abs_err_conf = parse_abs(func)
        for i in range(repetitions):
            integrals[i], abs_err, rel_err, times[i] = list(map(float, run_executable(func, threads, points).strip().split("\n")))

        if all(abs(i - j) < abs_err_conf for i, j in combinations(integrals, 2)):
            results += [f"{integrals[0]}\n{abs_err}\n{rel_err}\n{int(min(times))}\n" +
                        f"{int(sum(times) / repetitions)}\n{int(scipy.stats.tstd(times)) if repetitions > 1 else 0}"]
        else:
            results += ["Different values of integers.", "\n"]
    print("\n\n".join(results))


def parser():
    if len(sys.argv) == 4:
        if int(sys.argv[1]) < 1 or int(sys.argv[2]) < 1 or int(sys.argv[3]) < 1 :
            print("Incorrect number of repetitions, threads or points.")
            exit(1)
        return int(sys.argv[1]), int(sys.argv[2]), int(sys.argv[3])
    else:
        print("Incorrect amount of argument.")
        exit(1)


def main():
    analyze(parser())


if __name__ == "__main__":
    main()
