src = $(wildcard src/*.c)
out = alfish.exe
cflags = -O3 -Isrc/

all: $(src)
	make clean
	make build
	make run
all_debug: $(src)
	make clean
	make debug
	make run
clean:
	del $(out)
	cls
build: $(src)
	gcc $(cflags) $(src) -o $(out)
run:
	$(out)
debug:
	gcc $(cflags) -g $(src) -o $(out)
