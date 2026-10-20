# Smart Airport Simulator & Custom Shell (C++)

An Operating Systems course project in two parts:

1. **Smart Airport Simulation** (`airport_sim.cpp`): a multi-threaded simulation of an airport hub. Flights compete for limited ground resources under a custom scheduler and a custom memory manager.
2. **Custom Shell** (`myshell.cpp`): a command-line shell with process creation and multi-stage pipes.

The full design, formulas, test plan and results are in [`Project_Report.md`](Project_Report.md).

## Airport simulation

Each flight operation runs as its own thread and competes for seven resource types: fuel trucks, baggage carts, catering trucks, cleaning crews, maintenance teams, jetbridges and de-icing trucks. Mutexes and condition variables control access, and every flight passes through a 5-stage passenger pipeline (check-in → security → customs → gate → boarding).

**HMFQ-PPRA scheduler.** A multilevel feedback queue extended with:
- a Predicted Impact Score that weighs delay, connections, resources, weather and fuel
- exponential aging to prevent starvation
- a dynamic quantum based on load
- priority inheritance, so a high-priority flight blocked on a resource isn't stuck behind lower-priority work
- cost-based preemption and a guaranteed-service promotion

**AWSC-PPC memory manager.** A working-set clock page-replacement algorithm with:
- a multi-pass victim score (frequency, recency, access pattern, prediction)
- predictive prefetching and frequency decay
- priority protection for important processes
- thrashing detection

## Custom shell

Runs programs with `fork()` + `execvp()` and connects any number of commands with `pipe()` + `dup2()`, such as `ls | grep cpp | wc -l`.

## Build and run

Linux:

```bash
g++ -std=c++17 -pthread -o airport airport_sim.cpp && ./airport
g++ -std=c++17 -o myshell myshell.cpp && ./myshell
```

## Concepts covered

Multithreading · synchronization (mutex, condition variables) · CPU scheduling · priority inversion · page replacement · process creation · IPC
