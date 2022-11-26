CC = g++



#Create a man page for both programs
man: mans manc

#Create a man page for server
mans:
	./server | groff -mandoc > manserver.pdf

#Create a man page for client
manc:
	./client | groff -mandoc > manclient.pdf

#Create both executable files.
all: server client

#Create both executable files for debugging.
allg: serverg clientg

#Create both executable files for optimization.
allo: servero cliento

#compile all object files, create executable './server'.
server: server.o tands.o
	$(CC) -o server server.o tands.o

#compile all object files, create executable './server' and './client' for debugging.
serverg: server.o tands.o
	$(CC) -o server server.o tands.o -g
	
#compile all object files, create executable './server' and './client' for optimization.
servero: server.o tands.o
	$(CC) -o server server.o tands.o -O

#compile all object files, create executable './client'.
client: client.o tands.o
	$(CC) -o client tands.o client.o

#compile all object files, create executable './server' and './client' for debugging.
clientg: client.o tands.o
	$(CC) -o client tands.o client.o -g
	
#compile all object files, create executable './server' and './client' for optimization.
cliento: client.o tands.o
	$(CC) -o client tands.o client.o -O

#create object file 'tands.o'.
tands.o: tands.cpp
	$(CC) -c tands.cpp

#create object file 'client.o'.
client.o: client.cpp
	$(CC) -c client.cpp

#create object file 'server.o'.
server.o: server.cpp
	$(CC) -c server.cpp

#delete all object files and executable.
clean:
	rm ./server server.o ./client client.o tands.o