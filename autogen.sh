#!/bin/sh

aclocal \
&& libtoolize \
&& automake --copy --gnu --add-missing \
&& autoconf
