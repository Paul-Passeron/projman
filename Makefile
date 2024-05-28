CC=gcc
CFLAGS=-Wall -Wextra -pedantic -g
LIBS=
SRC=src/
BUILD=build/
BIN=bin/

DEPS=  $(BUILD)projman.o
all: init lines projman
lines:
	@echo "C:"
	@wc -l $$( find -wholename './*.[hc]') | tail -n 1
$(BUILD)%.o: $(SRC)%.c
	 $(CC) $(CFLAGS) -o $@ $^ -c
$(BIN)projman: $(DEPS)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)
projman: $(BIN)projman
clean:
	rm -rf $(BIN)*
	rm -rf $(BUILD)*
install:
	cp $(BIN)projman /bin/
init:
	