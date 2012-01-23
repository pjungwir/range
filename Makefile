# For the gcc call, we need `-lm` on Ubuntu to get the math functions,
# but this is not necessary on OS/X. This would be a perfect project
# to learn automake/autoconf to figure out what parameters are needed.

build: range

range: range.o
	gcc range.o -lm -o range

experiment: experiment.o
	gcc experiment.o -lm -o experiment

test: build
	./test-range.pl

readme:
	markdown README.md > README.html

clean:
	rm -f *.o range experiment
