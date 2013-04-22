dec_server: io.o graph.o main.o server.o
	gcc -o dec_server io.o graph.o main.o server.o

io.o: io.c
	gcc -c -Wall io.c

graph.o: graph.c
	gcc -c -Wall graph.c

main.o: main.c
	gcc -c -Wall main.c -lpthread

server.o: server.c
	gcc -c -Wall server.c -lpthread

