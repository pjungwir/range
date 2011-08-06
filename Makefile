# For the gcc call, we need `-lm` on Ubuntu to get the math functions,
# but this is not necessary on OS/X. This would be a perfect project
# to learn automake/autoconf to figure out what parameters are needed.

build:
	gcc range.c -lm -o range

test: build
	./test-range.pl

clean:
	rm -f *.o range
