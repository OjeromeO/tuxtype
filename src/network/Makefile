CC = gcc
LFLAGS = -g -W -Wall -Wmissing-declarations -Wmissing-prototypes -Wredundant-decls -Wshadow -Wbad-function-cast -Wcast-qual
CFLAGS = -g -W -Wall -Wmissing-declarations -Wmissing-prototypes -Wredundant-decls -Wshadow -Wbad-function-cast -Wcast-qual
LIBS = `sdl-config --libs` -lSDL_net
INCS = `sdl-config --cflags`
SRC = servermain.c clientmain.c
OBJ = $(SRC:.c=.o)
EXEC = server client



all : $(EXEC)

server : servermain.o
	$(CC) -o $@ $^ $(LIBS) $(LFLAGS)
servermain.o : servermain.c
	$(CC) $(INCS) $(CFLAGS) -c $<

client : clientmain.o
	$(CC) -o $@ $^ $(LIBS) $(LFLAGS)
clientmain.o : clientmain.c
	$(CC) $(INCS) $(CFLAGS) -c $<

clean :
	@ rm -f $(OBJ) *~

mrproper : clean
	@ rm -f $(EXEC)
