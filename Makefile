CC = g++
OBJECTS = server.o

#compile all object files, create executable './prodcon'.
run: $(OBJECTS)
	$(CC) -o server $(OBJECTS)
	
server.o: $(OBJECTS)
	$(CC) -c server.cpp

#delete all object files and executable.
clean:
	rm ./server server.o