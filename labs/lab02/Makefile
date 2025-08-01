CC = gcc
CFLAGS = -Wall -Wextra -g

# Targets
TARGET = test_hello
SOURCE = test_hello.c

# Build the program
$(TARGET): $(SOURCE)
	$(CC) $(CFLAGS) -o $(TARGET) $(SOURCE)

# Clean build artifacts
clean:
	rm -f $(TARGET)

.PHONY: test clean install uninstall