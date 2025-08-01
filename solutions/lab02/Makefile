CC = gcc
CFLAGS = -Wall -Wextra -g

# Targets:
TARGETS = test_hello test_calc test_reverse_string

all: $(TARGETS)

# Build the program
$(objects): %: %.c
	$(CC) $(CFLAGS) -o $@ $<

# Clean build artifacts
clean:
	rm -f $(TARGETS)

.PHONY: test clean install uninstall