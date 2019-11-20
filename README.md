# Cache Simulator
 This program simulates a CPU cache in C.

## Status
 - Reads in files correctly.
 - Processes data correctly.
 - Input and output format is essentially correct.
   - professor suggested that the default notifications be printed as `(default)` after each corresponding printed input instead
 - Cache is created successfully.
 - Simulation runs successfully, verified that calculations are pretty much correct if not exact.
 - No leaks! 🙂

## Known Bugs
 - Linux
   - After much sweat and tears, none. Verified using `valgrind`.
 - Windows
   - None. Fixed a small bug where the null terminator was written one byte out of bounds.
 
## Extras
 - The file `extras/stuff.c` contains an alternative way to read in the file and process the trace data.
   - Known Bugs:
     - readFileToString(): none
     - convertDataFromStr(): there are 2 extra reads (presumably by `strtok_r()`) starting after the allocated copy of the input file (still works normally if the program is run using `valgrind`)
