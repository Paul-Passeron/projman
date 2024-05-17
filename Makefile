CC=gcc
CFLAGS=-Wall -Wextra -pedantic
SRC=src/
BUILD=build/
BIN=bin/

DEPS=  $(BUILD)projman.o
all: lines projman
lines:
	@echo "C:"
	@wc -l $$( find -wholename './*.[hc]') | tail -n 1
$(BUILD)%.o: $(SRC)%.c
	 $(CC) $(CFLAGS) -o $@ $^ -c
$(BIN)projman: $(DEPS)
	$(CC) $(CFLAGS) -o $@ $^
projman: $(BIN)projman
clean:
	rm -rf $(BIN)*
	rm -rf $(BUILD)*
install:
	cp $(BIN)projman /bin/