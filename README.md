# Ring Allreduce

This is a small C and MPI project.

The project tests the Ring-Allreduce algorithm. It also has a simple
baseline. The baseline uses `MPI_Reduce` and `MPI_Bcast`.

The code does not train a real AI model. It uses dummy float arrays. Each
rank fills its array with `1.0`. After the reduce step, each value must be
equal to the number of ranks.

## What This Project Does

- Builds two MPI programs:
  - `baseline`
  - `ring`
- Sends float arrays between MPI ranks.
- Checks that the final data is correct.
- Measures communication time with `MPI_Wtime`.
- Can run many tests with one script.
- Can make result plots with Python.

## What Is Ring-Allreduce?

Allreduce is a group operation.

Each rank has data. All ranks need the sum of all data. After Allreduce, every
rank has the same final data.

Ring-Allreduce puts ranks in a ring.

Example:

```text
rank 0 -> rank 1 -> rank 2 -> rank 3 -> rank 0
```

The array is split into chunks. The algorithm has two parts:

1. Reduce-scatter
2. Allgather

In reduce-scatter, chunks move around the ring and ranks add values.

In allgather, the finished chunks move around the ring again. At the end, each
rank has the full result.

This helps because there is no one busy master rank. Each rank talks only to
its left and right rank.

## Use In Big AI Training

Big AI models often train on many GPUs or many machines. Each GPU reads some
training data. Then each GPU computes gradients.

Before the next training step, all GPUs need the same gradients. Ring-Allreduce
can share and sum these gradients. It sends gradient chunks around the ring.
This can make large model training faster, because one GPU does not need to
collect all data alone.

## Project Files

```text
.
|-- include/                  header files
|-- src/                      C source files
|   |-- baseline.c            MPI_Reduce + MPI_Bcast version
|   |-- ring_allreduce.c      Ring-Allreduce version
|   `-- common.c              shared helper code
|-- scripts/
|   |-- test_runner.sh        runs the benchmark sweep
|   `-- parse_results.py      reads CSV data and makes plots
|-- docs/
|   |-- checkpoint_report/    checkpoint report source
|   |-- final_report/         final report source
|   `-- presentation/         presentation HTML
|-- Makefile
`-- README.md
```

## Requirements

You need:

- C compiler
- OpenMPI
- `make`
- Python 3
- `matplotlib` for plots

On Ubuntu, you can install the main tools with:

```bash
sudo apt update
sudo apt install build-essential openmpi-bin libopenmpi-dev python3 python3-matplotlib
```

## Build

Run:

```bash
make all
```

This creates:

```text
bin/baseline
bin/ring
```

To remove build files:

```bash
make clean
```

## Run One Test

Run the baseline:

```bash
mpirun --oversubscribe -np 4 ./bin/baseline 1048576
```

Run Ring-Allreduce:

```bash
mpirun --oversubscribe -np 4 ./bin/ring 1048576
```

The last number is the array size in float elements.

A good run prints:

```text
Verification: PASS
```

## Run All Benchmarks

Run:

```bash
./scripts/test_runner.sh
```

The script tests:

- 2, 4, 8, and 16 ranks
- array sizes from 256 KB to about 100 MB
- baseline and ring programs
- 5 timed runs after 1 warmup run

It writes:

```text
results/results.csv
results/runner.log
```

## Make Plots

After the benchmark script, run:

```bash
python3 scripts/parse_results.py
```

This writes plots to:

```text
results/plots/
```

## Profile

To build a `gprof` version:

```bash
make profile
```

Then run one program and read the profile:

```bash
mpirun --oversubscribe -np 4 ./bin/ring 4194304
gprof ./bin/ring gmon.out
```

Profiling is only for study. It is not used for benchmark times.

## Main Result

The final report shows that Ring-Allreduce is faster than the baseline in the
tested cases.

The measured speedup is from `1.26x` to `3.71x`.

The largest tested array has about 100 MB of float data. In that test, the ring
method still gives a clear speedup.


## Team

- Muhammed Fatih Asan
- Sarper Avci
- Mehmet Furkan Dalfidan
