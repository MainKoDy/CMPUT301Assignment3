CC = g++
OBJECTS = server.o

#compile all object files, create executable './prodcon'.
run: $(OBJECTS) tands.o
	$(CC) -o server $(OBJECTS) tands.o

client: client.o tands.o
	g++ -o client client.o tands.o
	
tands.o: tands.o
	g++ -c tands.cpp

client.o: client.o
	g++ -c client.cpp
server.o: $(OBJECTS)
	$(CC) -c server.cpp

#delete all object files and executable.
clean:
	rm ./server server.o ./client client.o

sclean:
	rm ./server server.o

cclean:
	rm ./client client.o