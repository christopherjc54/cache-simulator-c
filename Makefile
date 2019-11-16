all:
	gcc src/main.c src/data.c src/queue.c src/simulation.c -o output/sim -lm
allw:
	gcc src/main.c src/data.c src/queue.c src/simulation.c -o output/sim -lm -w
runtiny:
	output/sim -f "$(CURDIR)/traces/TinyTrace.trc"
runtest:
	output/sim -f "$(CURDIR)/traces/TestTrace.trc"
clean:
	rm output/* -f