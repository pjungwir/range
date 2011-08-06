/*
 * range.c - prints the requested range of values.
 *
 * Takes a start and stop position, and prints all values from one to the other (inclusive).
 * For example, given 3 and 6, range will print 3 4 5 6.
 * This can be useful for looping over numbered files in the shell.
 * For instance, if you want to shrink the wedding photos from your rehearsal dinner, you could say:
 *
 *   for t in `range 342 419`; do
 *     convert wedding-$t.jpg -resize 300x300 wedding-www-$t.jpg;
 *   done;
 *
 * In shell code, this approach is more concise than controlling your loop with arithmetic.
 * But if you have to iterate over huge ranges, it may not be as practical.
 *
 * Range can count forward or backwards, over positive or negative numbers.
 * By default it uses base 10 values, but you can use options to change inputs/outputs to
 * binary (-b), octal (-o), or hexadecimal (-x or -X).
 *
 * You can also use an alphabetic counting system (-a or -A), which works like this:
 *   a b c ... x y z aa ab ac ... ax ay az ba bb ... zz aaa aab ...
 * This system still accepts negative values, but there is no "zero";
 * it goes straight from -a to a.
 *
 * In cases where range must print letters (hex and alphabetic output), you can get lowercase output
 * by using the lowercase option (-x and -a) or uppercase output by using the uppercase option (-X and -A).
 *
 * If either start or stop value begins with a zero, then range will pad all output values with enough zeroes
 * to ensure that all values have the same number of digits as the longest input value.
 *
 * There is also a forward option (-f). If you choose this option and your starting position is greater
 * than your ending, then instead of counting backwards, range will print nothing. If you put this
 * in a shell's for-loop, you'll get zero iterations. For instance, this will print nothing:
 *
 *   for t in `range -f 7 4`; do
 *     echo $t;
 *   done;
 *
 * By default, range separates each number with a space when printing to a terminal,
 * otherwise with a newline. You can force spaces with the -s option or newlines with the -S option.
 *
 * You can see a brief usage message by running range with the -h or -? options or with no arguments.
 *
 * IMPLEMENTATION
 *
 * Range works by converting the input to base 10, iterating from start to stop,
 * and converting each value back to the original base. When using the -a or -A options,
 * this is a little more complicated. I try to act like I'm just in a base-26 system,
 * with a=1, b=2, etc., but the notation doesn't quite work like normal base-n systems.
 * Consider than in base 8, 8 is written 10, and in base 4, 4 is written 10,
 * and in base n, n is written 10. But in our "base-26" alphabetic system,
 * 26 is written z. Also there is no zero. We go from z straight to aa.
 * So there are a few special checks for base-26 operation hidden in the algorithms.
 *
 * Copyright 2009 by Paul A. Jungwirth
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <limits.h>

static char digits[17] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f', '\0'};

// Assumes str has enough space allocated to accomodate the extra length!:
void pad_with_zeros(char *str, int n) {
  char *tmp = strdup(str);
  char *buf = malloc(n+1);
  buf[n] = '\0';
  memset(buf, '0', n);
  sprintf(str, "%.*s%s", n - strlen(tmp), buf, tmp);
  free(tmp);
}

void help(int err) {
	fprintf(stderr, "USAGE: range [options] start end\n");
	exit(err);
}

void memfail() {
	fprintf(stderr, "Memory allocation error!\n");
	exit(EXIT_FAILURE);
}

int abs_max(int a, int b) {
	return abs(a) >= abs(b) ? abs(a) : abs(b);
}

int convert_to_10(int base, char* from, int* to) {
	// TODO: use strtol(3)??
	int i, n, v;
	char c;
	int mult = 1;
	n = strlen(from);
	if (n == 0) return 1;
	// avoid overflows writing into to:
	if (pow(base, n-1) > INT_MAX) return 1;

	// if converting a negative value, just shift the string pointer and the strlen by one:
	if (from[0] == '-') {
		if (n == 1) return 1;
		mult = -1;
		from++;
		n--;
	}

	*to = 0;
	for (i = 0; i < n; i++) {
		c = from[n-i-1];
		if (base == 26) {
			if (c >= 65 && c <= 90) v = c - 64;			// treat A as 1
			else if (c >= 97 && c <= 122) v = c - 96;	// treat a as 1
			else return 1;								// note zeroes are also excluded
			if (v > base) return 1;
		} else {
			if (c >= 48 && c <= 57) v = c - 48;			// 0 - 9
			else if (c >= 65 && c <= 70) v = c - 55; 	// A - F
			else if (c >= 97 && c <= 102) v = c - 87; 	// a - f
			else return 1;								// not a number
			if (v >= base) return 1;
		}
		*to += v * pow(base, i);
	}
	*to *= mult;
	return 0;
}

// Returns the number of digits required to represent a given value in a given base.
// Works only for values > 0!
int digits_of(int base, int value) {
	int s = 0, i = 0;
	if (base == 26) {
		while (s < value) {
			s += 26 * pow(26, i);
			i++;
		}
		return i;
	} else {
		// careful, this will segfault if value == 0:
    // Broken because of float imprecision for numbers like 1000b:
		// return (log(value) / log(base)) + 1;	// log(a,b) = log(c,b) / log(c,a)
    i = 1;
    while (value /= base) i++;
    return i;
	}
}

void convert_from_10(int base, int from, char* to) {
	int i;
	int n;
	int mod;

	if (from < 0) {
		to[0] = '-';
		convert_from_10(base, -from, to+1);
	} else if (from == 0) {
		to[0] = '0';
		to[1] = '\0';
	} else {
		n = digits_of(base, from);
    // printf("%d\n", n);
		// work from the least-significant digits up (i.e. right to left):
		for (i = n-1; i >= 0; i--) {
			if (base == 26) {
				mod = from % 26;
				if (mod == 0) {		// This means we've got a z.
					mod = 26;		// Count it as 26 and subtract it from the current value.
					from -= 26;
				}
				to[i] = 96 + mod;
			} else {
				to[i] = digits[from % base];
			}
			from /= base;
		}
		to[n] = '\0';
	}
}

void strupper(char *s) {
	int i = 0;
	while (s[i]) {
		s[i] = toupper(s[i]);
		i++;
	}
}

int main(int argc, char **argv) {
	int i = 1, j;
	int len;
	char *a;
	int base = 0;
	int start, stop;
	char *s;
	char *fmt;
	char separator;
	int capitalize = 0;
	int forward_only = 0;
  int zero_padding = 0;

	// Print w/ newlines unless outputting to a terminal (like ls):
	separator = isatty(1) ? ' ' : '\n';

	while (i < argc - 2) {
		a = argv[i];
		len = strlen(a);
		if (len < 2) break;
		if (a[0] != '-') break;
		for (j = 1; j < len; j++) {
			switch (a[j]) {
				case 'X':
					capitalize = 1;
					// fall through
				case 'x':
					if (base != 0) help(EXIT_FAILURE);
					base = 16;
					break;
				case 'o':
					if (base != 0) help(EXIT_FAILURE);
					base = 8;
					break;
				case 'b':
					if (base != 0) help(EXIT_FAILURE);
					base = 2;
					break;
				case 'A':
					capitalize = 1;
					// fall through
				case 'a':
					if (base != 0) help(EXIT_FAILURE);
					base = 26;
					break;
				case 'f':
					forward_only = 1;
					break;
				case 's':
					separator = ' ';
					break;
				case 'S':
					separator = '\n';
					break;
				// TODO: option to use any base.
				case 'h':
				case '?':
					help(EXIT_SUCCESS);
				default:
					help(EXIT_FAILURE);
			}
		}
		i++;
	}
	if (i + 2 != argc) help(EXIT_FAILURE);
	if (base == 0) base = 10;

	// If the input has leading zeroes, put them on the output, too:
	if (argv[i][0] == '0' || argv[i+1][0] == '0') {
    zero_padding = abs_max(strlen(argv[i]), strlen(argv[i+1]));
		// asprintf(&fmt, "%%0%ds%c", abs_max(strlen(argv[i]), strlen(argv[i+1])), separator);
    // printf("%s\n", fmt);
	} else {
		// asprintf(&fmt, "%%s%c", separator);
	}
  asprintf(&fmt, "%%s%c", separator);
	if (fmt == NULL) memfail();

	if (convert_to_10(base, argv[i], &start)) help(EXIT_FAILURE);
	if (convert_to_10(base, argv[i+1], &stop)) help(EXIT_FAILURE);

	// printf("%d %d\n", start, stop);
	// printf("%d\n", abs_max(start, stop));
	// printf("%d\n", digits_of(base, abs_max(start, stop)));
	
	// Now write each value of the count in the same base as the input:
	
	// Allocate one extra byte for a minus sign and one for the \0:
	s = malloc(sizeof(char) * (digits_of(base, abs_max(start, stop)) + 2));
	if (s == NULL) memfail();

	if (start <= stop) {
		for (i = start; i <= stop; i++) {
			if (i == 0 && base == 26) continue;
			convert_from_10(base, i, s);
			if (capitalize) strupper(s);
      if (zero_padding) pad_with_zeros(s, zero_padding);
			printf(fmt, s);
		}
	} else {
		if (forward_only) {
			printf("%c", separator);
		} else {
			for (i = start; i >= stop; i--) {
				if (i == 0 && base == 26) continue;
				convert_from_10(base, i, s);
				if (capitalize) strupper(s);
      if (zero_padding) pad_with_zeros(s, zero_padding);
				printf(fmt, s);
			}
		}
	}
	if (separator == ' ') puts("");	// end with a newline if we haven't printed one yet.
	free(s);
	free(fmt);

	return EXIT_SUCCESS;
}

