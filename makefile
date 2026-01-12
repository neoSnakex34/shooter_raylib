CC = gcc
CFLAGS = -Wall -Wextra -O2 -I ./raylib-5.5_linux_amd64/include
LDFLAGS = -L ./raylib-5.5_linux_amd64/lib -lraylib

TARGET = game
SRC = $(filter-out main.c, $(wildcard *.c))

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET) $(LDFLAGS)

clean:
	rm -f $(TARGET)
