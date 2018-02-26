#fisier folosit pentru compilarea serverului&clientului UDP

all:
	gcc server.c -o server -lsqlite3
	gcc client.c -o client -lsqlite3
clean:
	rm -f *~client server
