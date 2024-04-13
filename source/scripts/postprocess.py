#!/usr/bin/env python3

import os
import sys
import shutil
from pathlib import Path

root = Path(sys.argv[1])
external = root / "externals" / "chuck~.mxo" / "Contents"
resources = external / "Resources"
examples = root / "examples"
external_examples = resources / "examples"


print('-'*80)

print(f"project root: {root}")
if not resources.exists():
	resources.mkdir()
	print(f"created: {resources}")
else:
	print("skipped creating Resources folder")

# if not external_examples.exists():
# 	shutil.copytree(examples, external_examples)
# 	print(f"copied {examples} to {external_examples}")
# else:
# 	print(f"skipped copying {examples} to {external_examples}")

print('-'*80)

