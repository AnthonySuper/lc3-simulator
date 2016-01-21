CC := gcc
CFLAGS := -g -Wall -std=gnu11
SRCDIR := src
LFLAGS := -Iinclude/
DEPS := $(wildcard $(SRCDIR)/*.c)
TEST_FILES := $(wildcard test/*.c)
OBJECTS := $(patsubst $(SRCDIR)/%,build/%,$(DEPS:.c=.o))

build/%.o: src/%.c $(DEPS)
	mkdir -p build/
	$(CC) -c -o $@ $< $(CFLAGS) $(LFLAGS)


bin/run: main.c $(OBJECTS) 
	$(CC) $(CFLAGS) $(LFLAGS) $^ -o bin/run
	
bin/test: $(TEST_FILES) $(OBJECTS)
	$(CC) $(CFLAGS) $(LFLAGS) -Itest/ test/main.c $(OBJECTS) -o bin/test

clean:
	rm -r build/ bin/run

.PHONY: clean
