#!/usr/bin/env python3

import sys
import platform
from pathlib import Path

PLATFORM = platform.system()

root = Path(sys.argv[1])

if PLATFORM == "Darwin":
	external = root / "externals" / f"chuck~.mxo" / "Contents"
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

