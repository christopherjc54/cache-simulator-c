all:
	mkdir -p output > /dev/null
	gcc src/main.c src/data.c src/queue.c src/simulation.c src/cache.c -o output/sim -lm -w
	gcc src/main_automated.c src/data.c src/queue.c src/simulation.c src/cache.c -o output/sim_auto -lm -lpthread -w
allwarn:
	mkdir -p output > /dev/null
	gcc src/main.c src/data.c src/queue.c src/simulation.c src/cache.c -o output/sim -lm
	gcc src/main_automated.c src/data.c src/queue.c src/simulation.c src/cache.c -o output/sim_auto -lm -lpthread
runauto:
	output/sim_auto -o "output/results.csv" -c "$(CURDIR)" -t 1 > output/log_auto.txt
runauto-mt:
	output/sim_auto -o "output/results.csv" -c "$(CURDIR)" -t 8 > /dev/null
runtiny:
	output/sim -f "$(CURDIR)/traces/TinyTrace.trc"
runtest:
	output/sim -f "$(CURDIR)/traces/TestTrace.trc"
runcorruption:
	output/sim -f "$(CURDIR)/traces/Corruption1.trc"
run1a:
	output/sim -f "$(CURDIR)/traces/Trace1A.trc"
run2a:
	output/sim -f "$(CURDIR)/traces/Trace2A.trc"
runa9:
	output/sim -f "$(CURDIR)/traces/A-9_new 1.5.pdf.trc"
clean:
	rm output/* -f > /dev/null