CC = gcc
CFLAGS = -g -std=c99 -O2 -Wall -finline-functions -Iinclude
LIBRARY_FLAGS = -Llib -lsat

EXEC_FILE = sat 
TEST_EXEC = unit_tests

SRC = src/main.c
TEST_SRC = test/test.c

OBJS=$(SRC:.c=.o)
TEST_OBJS = $(TEST_SRC:.c=.o)

HEADERS = sat_api.h

sat: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(LIBRARY_FLAGS) -o $(EXEC_FILE)

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@
	
test: $(TEST_OBJS) $(TEST_SRC)
	$(CC) $(CFLAGS) $(TEST_OBJS) $(LIBRARY_FLAGS) -o $(TEST_EXEC)

runtests: test
	./$(TEST_EXEC)
clean:
	rm -f $(OBJS) $(TEST_OBJS) $(EXEC_FILE) $(TEST_EXEC)	
