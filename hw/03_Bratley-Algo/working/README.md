# HW3 - Parallel Branch and Bound: Bratley Algo

## 1 Script

```sh
cmake * # install dependecies
make # compile binnary
mpirun -np 4 ./Main # run binary woth mpi
make && mpirun -np 4 ./Main # compile binay and run it
```

## 2 Assigement

The task is to implement parallel branch-and-bound algorithm for Bratley’s problem using MPI. There are 3 elimination rules:

### 2.1 Missed deadline

It might happen that unassigned task would miss its deadline when assigned to the current schedule, if that is the case, prune this node.

*It is meaningless to continue, because in the future, some task would surely miss its deadline.*

$(∃Tj ∈ V : max\{c, rj\} + pj > dj )$ => prune this node

- $c$ – length of the partial schedule
- $V$ – a set of non-scheduled tasks

## 2.2 Bound on the solution

We might have already found some feasible solution, which might not be optimal. However, we can use its quality as an upper bound (`UB`). We can calculate lover bound (`LB`) of the current solution and prune this node if `LB ≥ UB`.

$LB = max\{c, \min_{T_j \in V}\{r_j\}\} + \sum_{T_j \in V}{p_j}$

- $c$ – length of the partial schedule
- $V$ – a set of non-scheduled tasks

## 2.3 Decomposition

We might be able to detect, that the partial solution we have in the current node is optimal, therefore it might not be necessary to backtrack.

$(c \leq \min_{T_j \in V}\{r_j\} )$ => prune this node

- $c$ – length of the partial schedule
- $V$ – a set of non-scheduled tasks

![Example for 4 tasks](example.png)