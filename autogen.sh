#!/bin/sh

aclocal \
&& libtoolize \
&& automake --gnu --add-missing \
&& autoconf
