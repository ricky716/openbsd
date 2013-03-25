#!/usr/bin/perl -w 

# check that simple requiring BigFloat and then bzero() works

use strict;
use Test::More tests => 1;

require Math::BigFloat;
my $x = Math::BigFloat->bzero(); $x++;
is ($x,1, '$x is 1');

# all tests done

