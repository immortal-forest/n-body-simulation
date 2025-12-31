CC = gcc
CFLAGS = -Wall -Wextra -O3
SRC = native/main.c
OBJ = build/galaxy

all: $(OBJ)

$(OBJ): $(SRC)
	$(CC) $(CFLAGS) -o $(OBJ) $(SRC) -lm

run: all
	./$(OBJ) > data/output.csv

clean:
	rm -f build/* data/*.csv
