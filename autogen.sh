#!/bin/sh

aclocal \
&& autoconf \
&& automake --gnu --add-missing \
&& libtoolize
