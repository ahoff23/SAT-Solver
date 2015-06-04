CC = gcc
CFLAGS = -std=c99 -O2 -Wall -finline-functions -Iinclude
AR = ar
AR_FLAGS = -cq
LIB_FILE = libsat.a

SRC = src/sat_api.c

OBJS=$(SRC:.c=.o)

HEADERS = sat_api.h Lists.h

sat: $(OBJS) $(SRC)
	$(AR) $(AR_FLAGS) $(LIB_FILE) $(OBJS)

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@
	
copy: $(LIB_FILE)
	cp $(LIB_FILE) ../sat_solver/lib/$(LIB_FILE)

clean:
	rm -f $(OBJS) $(LIB_FILE)
