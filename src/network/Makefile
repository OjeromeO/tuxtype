CC = gcc
LFLAGS = -g -W -Wall -Wmissing-declarations -Wmissing-prototypes -Wredundant-decls -Wshadow -Wbad-function-cast -Wcast-qual
CFLAGS = -g -W -Wall -Wmissing-declarations -Wmissing-prototypes -Wredundant-decls -Wshadow -Wbad-function-cast -Wcast-qual
LIBS = `sdl-config --libs` -lSDL_net
INCS = `sdl-config --cflags`
SRC = server.c client.c network.c
OBJ = $(SRC:.c=.o)
EXEC = server client



all : $(EXEC)

server : server.o network.o
	$(CC) -o $@ $^ $(LIBS) $(LFLAGS)
client : client.o network.o
	$(CC) -o $@ $^ $(LIBS) $(LFLAGS)

server.o : server.c network.h
	$(CC) $(INCS) $(CFLAGS) -c $<
client.o : client.c network.h
	$(CC) $(INCS) $(CFLAGS) -c $<
network.o : network.c network.h
	$(CC) $(INCS) $(CFLAGS) -c $<

clean :
	@ rm -f $(OBJ) *~

mrproper : clean
	@ rm -f $(EXEC)
