#!/usr/bin/env python3

import shutil
from pathlib import Path

for folder in Path('source/projects/chugins').iterdir():
    if folder.is_dir():
        for f in folder.iterdir():
            if f.suffix == '.ck':
                print(f'copying {f} to examples folder')
                shutil.copy(f, Path('./examples'))
