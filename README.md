# Cache Simulator
 This program simulates a CPU cache in C.

## Status
 - Reads in files correctly.
 - Processes data correctly.
 - Input and output format is mostly correct.
   - professor suggested that the default notifications be printed as `(default)` after each corresponding printed input instead

## Known Bugs
 - After much sweat and tears, none.
 
## Extras
 - The file `garbage/stuff.c` contains an alternative way to read in the file and process the trace data.
   - Known Bugs:
     - readFileToString(): none
     - convertDataFromStr(): there are 2 extra reads (presumably by `strtok_r()`) starting after the allocated copy of the input file (still work normally if the program is run using `valgrind`)
