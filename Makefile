CC = g++
OBJECTS = server.o

#compile all object files, create executable './prodcon'.
run: $(OBJECTS)
	$(CC) -o server $(OBJECTS)

client: client.o
	g++ -o client client.o
	
client.o: client.o
	g++ -c client.cpp
server.o: $(OBJECTS)
	$(CC) -c server.cpp

#delete all object files and executable.
clean:
	rm ./server server.o ./client client.o