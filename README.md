# Cache Simulator
 This program simulates a CPU cache in C.

## Status
 - Reads in files correctly.
 - Processes data correctly.
 - Input and output format is mostly correct.
   - professor suggested that the default notifications be printed as `(default)` after each corresponding printed input instead.

## Known Bugs
- enqueue() fails to allocate memory when running the 2nd smallest trace file on Windows
  - smallest trace file work fine and memory usage on others is minimal, so not sure why this happens
- program crashes after running the 3rd smallest trace file on Linux
  - strongly suspect that it has to do with a poor file reading method after running it using valgrind
