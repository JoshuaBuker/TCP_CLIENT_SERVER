CC = g++
CFLAGS = -g -Wall
LIBS = -lwsock32 -lws2_32

SRCS = Server.cpp
OBJS = $(SRCS:.cpp=.o)

EXEC = Server

all: $(EXEC)

$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(EXEC)
