# source files
SRC = init.c ../../apps/fs_test.c
OBJ = $(SRC:.c=.o)
TARGET = init

# compiler and flags
CC = i686-elf-gcc
CFLAGS = -m32 -nostdlib -nostdinc -fno-builtin -fno-stack-protector \
         -nostartfiles -nodefaultlibs -Wall -Wextra -Werror -I../../include

# build rules
all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) -T ../../linker.ld -o $@ $^ $(CFLAGS) -lgcc

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)

.PHONY: all clean
