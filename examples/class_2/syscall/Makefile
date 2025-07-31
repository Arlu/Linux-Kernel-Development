CC = gcc
CFLAGS = -Wall -Wextra -g

# Targets
TARGET = write_stdout_syscall
SOURCE = write_stdout_syscall.c

# Build the program
$(TARGET): $(SOURCE)
	$(CC) $(CFLAGS) -o $(TARGET) $(SOURCE)

# Clean build artifacts
clean:
	rm -f $(TARGET)

# Show assembly output
disasm: $(TARGET)
	objdump -d $(TARGET) | grep -A 20 "write_stdout_syscall"

.PHONY: test clean install uninstall disasm debug