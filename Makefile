all: folders server client fifo

server: bin/monitor

client: bin/tracer

fifo: bin/fifo

folders:
	@mkdir -p src obj bin tmp includes 

bin/fifo: obj/fifo.o
	gcc -g obj/fifo.o -o bin/fifo

obj/fifo.o: src/fifo.c
	gcc -Wall -g -c src/fifo.c -o obj/fifo.o

bin/monitor: obj/monitor.o
	gcc -g obj/monitor.o -o bin/monitor

obj/monitor.o: src/monitor.c includes
	gcc -Wall -g -c -I includes src/monitor.c -o obj/monitor.o

bin/tracer: obj/tracer.o
	gcc -g obj/tracer.o -o bin/tracer

-o obj/tracer.o: src/tracer.c includes
	gcc -Wall -g -c -I includes src/tracer.c -o obj/tracer.o

clean:
	rm -f obj/* tmp/* bin/{tracer,monitor,fifo}