# -*- coding: utf-8 -*-
from __future__ import print_function
from __future__ import unicode_literals
from __future__ import absolute_import
from __future__ import generators
from __future__ import division

import ezdxf
import re
import sys

if len(sys.argv) != 3:
    print("Invalid arguments: " + str(sys.argv))
    exit(1)

dwg = ezdxf.readfile(sys.argv[1], encoding='utf-8')

print(dwg.dxfversion)
old_enc_versions = ['AC1009', 'AC1015', 'AC1018']
if dwg.dxfversion in old_enc_versions:
    dwg = ezdxf.readfile(sys.argv[1], encoding='cp932')

texts = []
block_pattern = r'\{\f.*;([^}]+)\}'
modelspace = dwg.modelspace()
for e in modelspace:
    if e.dxftype() == 'TEXT':
        texts.append(e.dxf.text)
    elif e.dxftype() == 'MTEXT':
        v = re.sub(r'\{\\f[^;]+;([^}]+)\}', r'\1', e.get_text()).replace('\\P', '\n')
        texts.append(v)

with open(sys.argv[2], 'w') as f:
    f.writelines([(t+'\n').encode('utf-8') for t in texts])

