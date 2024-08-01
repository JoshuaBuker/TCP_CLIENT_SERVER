CC = g++
CFLAGS = -g -Wall
LIBS = -lwsock32 -lws2_32

SRCS = Client.cpp
OBJS = $(SRCS:.cpp=.o)

EXEC = Client

all: $(EXEC)

$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(EXEC)
