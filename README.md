# EinStein würfelt nicht! (EWN)
EWN is a board game featuring schotastic process. Please refer to [Wikipedia](https://en.wikipedia.org/wiki/EinStein_w%C3%BCrfelt_nicht!) for more information.

In this project, the rules of EWN are slightly changed:
- Only one person is involved.
- A piece is allowed to move to all of eight directions.
- The goal is to move a specific piece to the given destination.

# How to run
- Compile:
    ```bash
    make verifier
    ```

- Usage:
    ```
    ./verifier [executable_program] [input_data]
    ```

- Usage example:
    ```
    ./verifier ./a.out input.in
    ```

# Algorithm
Two different optimized search algorithms are implemented, Iterative Deepening Limited Depth DFS and A* algorithm with heuristics.

## Iterative Deepening Limited Depth DFS
## A* w/ heuristics