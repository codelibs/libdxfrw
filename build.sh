#!/bin/bash

autoreconf -vfi
./configure --prefix=/opt/dxfrw
make
make install
make clean
cp bin/* /opt/dxfrw/bin
cd /opt
tar zcvf /work/dxfrw.tar.gz dxfrw
chmod 777 /work/dxfrw.tar.gz
