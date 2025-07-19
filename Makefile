CC = gcc
CFLAGS = -Wall -std=c99 -O2 `pkg-config --cflags raylib`
LDFLAGS = `pkg-config --libs raylib` -lm
SRC = src/main.c src/bot.c
BIN = Arkanoid

all: $(BIN)

$(BIN): $(SRC)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

run: $(BIN)
	./$(BIN)

clean:
	rm -f $(BIN) && clear