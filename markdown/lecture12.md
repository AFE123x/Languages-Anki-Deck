# 12. Analyzing Build

## buildroot - available tools to analyze builds

- buildroot offers multiple tools for analyzing builds:
    - **licensing report**: summarize the packages and their licenses
    - **dependency graphing** tools
    - **buildtime graphing** tools
    - **filesystem size** tools

## buildroot - dependency graphing

- This willl create a dependency graph.
    - `make graph-depends` will create the full dependency graph
    - `make <pkg>-graph-depends` will make the dependency graph of a given graph.
- The resulting graph will be in `$(O)/graphs/`

## buildroot - build time graphing

- this will make a bar chart of the compile times for packages (to analyze if any of them are causing issues).
    - it'll break down the times based on the steps.
- `make graph-build` will build the time graph, located in `$(O)/graphs/`

## buildroot - filesystem size graphing

- here, we can analyze the size of the root filesystem, which could let us deduce which packages are consuming the most space.
    - This lets us make size optimizations on the relevant packages

- `make graph-size` will produce .csv files, and a pie chart showing the comparisons.
