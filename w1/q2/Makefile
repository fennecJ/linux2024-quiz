CC = gcc
CFLAGS = -Wall -Wextra -std=c99
LDFLAGS =
PYTHON = python3

SRC = $(wildcard *.c)
HDR = $(wildcard *.h)
OBJ = $(SRC:.c=.o)

all: sort

sort: $(OBJ)
	$(CC) $(LDFLAGS) -o $@ $^

%.o: %.c $(HDR)
	$(CC) $(CFLAGS) -c -o $@ $<

compare: all
	@if ! python3 -c "import matplotlib.pyplot" >/dev/null 2>&1; then \
		echo "matplotlib.pyplot library is not installed. Please install it."; \
	else \
		./sort; \
		$(PYTHON) gen_stats_graph.py; \
	fi

clean:
	rm -f $(OBJ) sort

.PHONY: all compare clean