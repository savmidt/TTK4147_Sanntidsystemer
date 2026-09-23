TARGET = taskA

CC = clang
# For taskB, use these in CDFLAGS: -g -fsanitize=thread
CFLAGS = -std=gnu11
LDFLAGS = -lpthread

SRCS = $(wildcard *.c)
OBJS = $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJS)