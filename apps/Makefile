# makefile for lumaos applications

# compiler and flags
CC = i686-elf-gcc
CFLAGS = -m32 -nostdlib -nostdinc -fno-builtin -fno-stack-protector \
         -nostartfiles -nodefaultlibs -Wall -Wextra -Werror \
         -I../../include -I../../include/libc -I../../include/gui -I../../include/ltk

# source files
GUI_DEMO_SRC = gui_demo.c
GUI_DEMO_OBJ = $(GUI_DEMO_SRC:.c=.o)

# default target
all: gui_demo

# GUI Demo
GUI_DEPS = ../../gui/desktop.o ../../gui/theme.o ../../gui/wm.o \
           ../../gui/ltk/window.o ../../gui/ltk/draw.o \
           ../../gui/ltk/widget.o ../../gui/ltk/widgets/button.o

gui_demo: $(GUI_DEMO_OBJ) $(GUI_DEPS)
	$(CC) -T ../../linker.ld -o $@ $^ $(CFLAGS) -lgcc

# pattern rule for compiling .c files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# clean
clean:
	rm -f *.o gui_demo

.PHONY: all clean
