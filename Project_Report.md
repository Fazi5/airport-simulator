# Technical Report: OS Concepts Integration in Custom Shell and Smart Airport Simulation

## Table of Contents
1. [Introduction](#1-introduction)
2. [Methodology](#2-methodology)
3. [Question 1: Custom Shell System](#3-question-1-custom-shell-system)
4. [Question 2: Smart Airport Operations Management System](#4-question-2-smart-airport-operations-management-system)
5. [Algorithm Analysis: Strengths & Weaknesses](#6-algorithm-analysis-strengths--weaknesses)
6. [Test Plan](#6-test-plan)
7. [Work Distribution](#7-work-distribution)
8. [Learning and Future Improvements](#8-learning-and-future-improvements)
9. [Conclusion](#9-conclusion)
10. [References](#10-references)

## List of Figures
- **Figure 1**: Pipelining Flowchart (dup/dup2 logic)
- **Figure 2**: Airport System Architecture

## List of Tables
- **Table 1**: Resource Constraints and Count
- **Table 2**: Project Test Plan and Results

---

## 1. Introduction
The objective of this project was to explore and implement fundamental and advanced Operating System concepts through two distinct challenges:
- **Custom Shell**: A command-line processor demonstrating process creation (`fork`), program execution (`exec`), and IPC through redirection (`dup2`) and pipes (`pipe`).
- **Smart Airport Simulation**: A high-fidelity, multi-threaded simulation of an international airport hub, integrating complex scheduling (HMFQ-PPRA) and memory management (AWSC-PPC) algorithms to manage concurrent flight operations under stress.

## 2. Methodology
Our approach followed a tiered implementation strategy:
1. **Requirements Analysis**: Deconstructing complex scenarios into modular components.
2. **Algorithm Design**: Translating mathematical formulas (PIS, AWSC scores) into logic-weighted C++ implementations.
3. **Multi-threaded Architecture**: Utilizing the POSIX thread model for real-time concurrency.
4. **Verification**: Stress-testing via random event injection and bottle-neck analysis.

## 3. Question 1: Custom Shell System

### 3.1 Design & Implementation
The shell operates as an infinite loop performing the sequence: Read $\rightarrow$ Tokenize $\rightarrow$ Execute. It handles standalone commands via direct `fork/exec` and manages pipelined sequences by recursively connecting file descriptors.

### 3.2 OS Concepts: dup() and dup2()
Standard streams (0, 1, 2) are manipulated to achieve piping. 

**Figure 1: Pipelining Flowchart**
```mermaid
graph LR
    P[Parent Shell] --> C1[Child 1: ls]
    P --> C2[Child 2: wc]
    C1 -- "stdout (FD 1)" --> Pipe((Pipe))
    Pipe -- "stdin (FD 0)" --> C2
    style Pipe fill:#f9f,stroke:#333
```
In this scenario, `dup2(pipe_write_end, 1)` redirects `ls` output, while `dup2(pipe_read_end, 0)` redirects `wc` input.

### 3.3 Algorithm: Shell Executive Loop
1. Initialize Shell Environment.
2. While `input != "quit"`:
    a. Display `myshell> ` prompt.
    b. Read string `S`.
    c. If `S` contains `|`, split into `commands[]`.
    d. For each command in `commands[]`:
        i. Create `pipe()`.
        ii. `fork()`.
        iii. In Child: `dup2` for I/O, `execvp()`.
        iv. In Parent: Store PID, close pipe ends.
    e. Wait for all PIDs.

## 4. Question 2: Smart Airport Simulation

### 4.1 System Architecture
Implemented as a multi-threaded system with four core managers:
- **Flight Ops**: Turns aircraft through states (Arrival $\rightarrow$ Ground $\rightarrow$ Departure).
- **HMFQ-PPRA Scheduler**: Manages CPU-like turn time for ground tasks.
- **AWSC-PPC Memory**: Simulates virtual memory usage for process state tracking.
- **Crisis Manager**: Injects stochastic events (Weather, Mechanical Failure).

### 4.2 HMFQ-PPRA Scheduling Algorithm
**Hybrid Multi-Level Feedback Queue with Predictive Priority Round-Robin and Aging.**

#### The 8 Layers of Implementation:
1.  **Layer 1 (MLFQ Foundation)**: 5 Queues (Emergency to Low) with dynamic feedback.
2.  **Layer 2 (Predictive Component)**: Uses the **Predicted Impact Score (PIS)**:
    $PIS = \alpha \cdot Delay + \beta \cdot Connection + \gamma \cdot Resource + \delta \cdot Weather + \epsilon \cdot Fuel$
3.  **Layer 3 (Exponential Aging)**: Starvation prevention using:
    $Age\_Increment = Base\_Rate \cdot e^{(Wait\_Time / T_{const})}$
4.  **Layer 4 (Dynamic Quantum)**: Quantum adjusted by Load Factor (current queue size).
5.  **Layer 5 (Priority Inheritance)**: When a high-priority flight is blocked (e.g., waiting for a Fuel Truck), the current resource holder is boosted to prevent priority inversion.
6.  **Layer 6 (Preemption)**: Triggered if $Preemption\_Benefit > 1.5 \cdot Preemption\_Cost$.
7.  **Layer 7 (Guaranteed Service)**: Any operation waiting > 20 mins is promoted to Queue 1.
8.  **Layer 8 (Adaptive Learning)**: Uses weighted moving averages to update time estimates for ground services.

### 4.3 AWSC-PPC Page Replacement Algorithm
**Advanced Working Set Clock with Predictive Prefetching and Adaptive Compression.**

#### The 8 Components:
1.  **Working Set Tracking**: Window $\Delta$ adjusted by process phase (e.g., I/O phase reduces window).
2.  **Multi-Pass Intelligent Clock**: A 3-pass scan that calculates a **Victim Score**:
    $Score = (Freq \cdot 0.3) + (Recency \cdot 0.3) + (Pattern \cdot 0.2) + (Prediction \cdot 0.2)$
3.  **Predictive Prefetching**: Detects sequential patterns and loads next $N$ pages.
4.  **Adaptive Compression**: Pages with low frequency are compressed using a mock RLE system to save physical frames.
5.  **Frequency Decay**: $F = F \cdot 0.9$ every 1000 units to avoid "ghost" high-frequency pages.
6.  **Priority Integration**: High-priority processes receive a $2.0x$ multiplier to their victim scores to protect their pages.
7.  **Thrashing Detection**: Monitors fault rate; if $>25\%$, it suspends low-priority processes.
8.  **Markov Chain Prediction**: Tracks transition probabilities between page accesses ($P_{ij}$).

---

### 5.3 System Level Algorithms (Procedural Form)

#### Algorithm 1: Custom Shell Execution Loop
1. **START**
2. Initialize signal handler (SIGINT).
3. **DO UNTIL** user inputs "quit":
    a. Output prompt "myshell> ".
    b. Read tokenized input.
    c. **IF** contains pipe `|`:
        i. Create `pipe(p)`.
        ii. `fork()` first child -> redirect `p[1]` to `stdout`.
        iii. `fork()` second child -> redirect `p[0]` to `stdin`.
    d. **ELSE**:
        i. `fork()` child -> `execvp()` command.
        ii. Parent `waitpid(pid)`.
4. **END**

#### Algorithm 2: Airport Operation Task Turnaround
1. **START**
2. Flight `f` requests `Jetbridge` and `Runway`.
3. **CALL** `process_passengers(f)` (Stages: Check-in, Security, Customs).
4. **WHILE** `resources` not available: **WAIT** on `condition_variable`.
5. **LOCK** `Resource_Mutex`.
6. Perform `Refueling` -> `Cleaning` -> `Boarding`.
7. **RELEASE** all held resources and signal `notify_all()`.
8. Update `Budget` based on landing fees.
9. **END**

### 5.1 Resource Constraints
The simulation manages 7 critical resource types with finite capacities:
**Table 1: Resource Constraints and Count**
| Resource | Count | Purpose |
|---|---|---|
| Fuel Trucks | 8 | Aircraft refueling |
| Baggage Carts | 12 | Luggage processing |
| Catering Trucks | 6 | Food supply |
| Cleaning Crews | 4 | Cabin turnaround |
| Maintenance Teams | 3 | Safety checks |
| Jetbridges | 10 | Passenger boarding |
| De-icing Trucks | 2 | Winter operations |

### 5.2 Passenger Pipeline
A 5-stage processing pipeline is simulated for every flight:
1. **Check-in** (2-5 mins)
2. **Security** (3-8 mins)
3. **Customs** (2-10 mins)
4. **Gate Arrival** (20 mins pre-flight)
5. **Boarding** (15-30 mins)

## 6. Algorithm Analysis: Strengths & Weaknesses

### 6.1 HMFQ-PPRA
- **Strengths**: 
    - **Fairness**: Exponential aging ensures low-priority tasks eventually execute.
    - **Responsiveness**: Layer 2 (PIS) allows the system to preemptively prepare for high-impact delays.
    - **Priority Inheritance**: Prevents priority inversion when high-priority flights are blocked by resource holders.
- **Weaknesses**: 
    - **Complexity**: Calculating PIS and aging for hundreds of concurrent operations increases CPU overhead.
    - **Tuning**: The effectiveness of the system highly depends on correctly weighting $\alpha, \beta, \gamma \dots$

### 6.2 AWSC-PPC
- **Strengths**:
    - **Efficiency**: Multi-pass clock combined with frequency decay filters out transient page accesses.
    - **Predictability**: Pattern detection reduces page faults before they occur.
- **Weaknesses**:
    - **Metadata Overhead**: Each page frame requires significant metadata (R, M, F, T, etc.), consuming kernel memory.
    - **Decompression Latency**: While saving space, decompression can spike CPU usage during sudden "hot" memory access bursts.

## 6. Test Plan

**Table 2: Project Test Plan and Results**
| Test Case | Inputs | Expected Result | Result |
|---|---|---|---|
| TC-01 | `ls -l | grep cpp | wc -l` | Correct count of cpp files | PASS |
| TC-02 | `quit` command | Shell terminates gracefully | PASS |
| TC-03 | Fuel Emergency | Op jumps to Queue 0 immediately | PASS |
| TC-04 | Thunderstorm Crisis | Cost detected, budget deduction | PASS |
| TC-05 | 50+ Concurrent Flights | System manages resources via wait-loops | PASS |

## 7. Work Distribution
- **C++ Core & Shell**: Student A
- **Scheduler & Memory Logic**: Student B
- **Crisis & Financial Systems**: Student C
- **Documentation & Diagrams**: Student D

## 8. Learning and Future Improvements
- Practical understanding of race conditions and the necessity of `std::mutex`.
- Intricacies of file descriptor inheritance in Linux.
- The trade-off between scheduling fairness and system throughput.
- Performance tuning: Managing logging verbosity in multi-threaded environments.

**Improvements**:
- **Algorithm**: Integrate a real RLE (Run-Length Encoding) library for the compression component of AWSC-PPC.
- **System**: Add a GUI using a library like Qt or ImGui for the airport simulation.

## 9. Conclusion
This project successfully demonstrated the application of theoretical OS concepts in real-world scenarios. The combination of a low-level shell and a high-level airport simulation highlights the versatility of system-level programming and the critical role of resource management.

## 10. References
- Silberschatz, A., Galvin, P. B., & Gagne, G. (2018). *Operating System Concepts*.
- Stevens, W. R., & Rago, S. A. (2013). *Advanced Programming in the UNIX Environment*.
- C++ Reference: std::thread, std::mutex.
