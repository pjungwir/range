range
=====

Prints the requested range of values.

Takes a start and stop position, and prints all values from one to the other (inclusive).
For example, given 3 and 6, range will print 3 4 5 6.
This can be useful for looping over numbered files in the shell.
For instance, if you want to shrink the wedding photos from your rehearsal dinner, you could say:

    for t in `range 342 419`; do
      convert wedding-$t.jpg -resize 300x300 wedding-www-$t.jpg;
    done;

In shell code, this approach is more concise than controlling your loop with arithmetic.
But if you have to iterate over huge ranges, it may not be as practical.

Range can count forward or backwards, over positive or negative numbers.
By default it uses base 10 values, but you can use options to change inputs/outputs to
binary (-b), octal (-o), or hexadecimal (-x or -X).

You can also use an alphabetic counting system (-a or -A), which works like this:
    a b c ... x y z aa ab ac ... ax ay az ba bb ... zz aaa aab ...
This system still accepts negative values, but there is no "zero";
it goes straight from -a to a.

In cases where range must print letters (hex and alphabetic output), you can get lowercase output
by using the lowercase option (-x and -a) or uppercase output by using the uppercase option (-X and -A).

If either start or stop value begins with a zero, then range will pad all output values with enough zeroes
to ensure that all values have the same number of digits as the longest input value.

There is also a forward option (-f). If you choose this option and your starting position is greater
than your ending, then instead of counting backwards, range will print nothing. If you put this
in a shell's for-loop, you'll get zero iterations. For instance, this will print nothing:

    for t in `range -f 7 4`; do
      echo $t;
    done;

By default, range separates each number with a space when printing to a terminal,
otherwise with a newline. You can force spaces with the -s option or newlines with the -S option.

You can see a brief usage message by running range with the -h or -? options or with no arguments.

IMPLEMENTATION
--------------

Range works by converting the input to base 10, iterating from start to stop,
and converting each value back to the original base. When using the -a or -A options,
this is a little more complicated. I try to act like I'm just in a base-26 system,
with a=1, b=2, etc., but the notation doesn't quite work like normal base-n systems.
Consider than in base 8, 8 is written 10, and in base 4, 4 is written 10,
and in base n, n is written 10. But in our "base-26" alphabetic system,
26 is written z. Also there is no zero. We go from z straight to aa.
So there are a few special checks for base-26 operation hidden in the algorithms.

AUTHOR
------
Copyright 2009 by Paul A. Jungwirth.
See LICENSE.txt for further details.
