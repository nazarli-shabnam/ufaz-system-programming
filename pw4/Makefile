# Makefile for circle program

CC      = gcc
CFLAGS  = -Wall -Wextra -std=c11
TARGET  = circle
OBJ     = main.o perimeter.o area.o

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $(OBJ)

main.o: main.c pi.h
	$(CC) $(CFLAGS) -c main.c

perimeter.o: perimeter.c pi.h
	$(CC) $(CFLAGS) -c perimeter.c

area.o: area.c pi.h
	$(CC) $(CFLAGS) -c area.c

# removes intermediate files only (.o)
clean:
	rm -f $(OBJ)

# removes .o files AND the final executable
mrproper: clean
	rm -f $(TARGET)



# Usage:
#     make         # builds circle
# 	./circle     # run it
# 	make clean   # remove .o files
# 	make mrproper  # remove .o files and circle
