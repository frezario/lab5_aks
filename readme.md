# Lab work 4: Parallelization using Thread Pool.

Authors (team): 

Demchuk Nazar: https://github.com/frezario

Oleksiuk Liubomyr: https://github.com/Liubavaa

### Prerequisites

C/C++ compiler, CMake, python3

### Compilation

```
./compile.sh --no-debug-build --optimize-build
```

### Usage

To run a script:
- that show the graph of points and time:
```
python3 estimate_points.py
```
- that show the graph of times with and without thread pool:
```
python3 plot.py
```
- that show the graph with acceleration:

```
python3 acceleration.py
```
To run the c++ program, compile it and run as:

```
./compile.sh --no-debug-build --optimize-build
 cd cmake-build-release/
./integrate_parallel_tpool ../func1.cfg 1 4 100
```

### Important!

To write a clean code, we've made a strong assumption that user will chose only those amount of points per interval, which is reasonable to build a rectangle interval with. That's because in the opposite case the computations used to determine the size of each subinterval become too complex and boilerplate. Also, the subintervals would be of different size and in some cases will be unrepresantable as rectangles.

We've used the implementation of Thread Pool from the book "C++ Concurrency in action" for a few reasons, including simplicity.

### Results

We've made an alternative to previous integral computating program using thread-safe queue. Although, the result is not better than previous one, we've learned another good general way to parallelize computations.

#### Determination of the optimal number of points

We took the optimal value of 100 from previous lab 

#### Compare with and without queue

We used determined number of points to compare programs with and without thread pool.

![image](https://user-images.githubusercontent.com/91615650/225736115-085bc1f4-d81e-4166-ba73-e08efc6b448d.png)

![image](https://user-images.githubusercontent.com/91615650/225736149-8e41c20a-61d6-4c48-9196-3172729c6fe3.png)

Here also, as we take the small number of points, script with queue works faster with more threads.

#### Acceleration of parallelization

![acceleration](https://user-images.githubusercontent.com/92572643/224486170-35c4c187-68fc-4c04-96b8-1cbfeab6d749.png)

As $n$ - the number of threads increase, the coefficient decreases due to dividing the function by $n$.
