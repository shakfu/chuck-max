#!/usr/bin/env python3

import filecmp
import os

def report_recursive_diff(dcmp):
    """
    Recursively reports differences found by a dircmp object.
    """
    # Files that are only in the left directory
    for name in dcmp.left_only:
        print(f"ONLY LEFT: {os.path.join(dcmp.left, name)}")

    # Files that are only in the right directory
    for name in dcmp.right_only:
        print(f"ONLY RIGHT: {os.path.join(dcmp.right, name)}")

    # Files that are in both directories but have different content
    for name in dcmp.diff_files:
        print(f"DIFFERENT FILES: {os.path.join(dcmp.left, name)} and {os.path.join(dcmp.right, name)}")

    # Recursively compare common subdirectories
    for sub_dcmp in dcmp.subdirs.values():
        report_recursive_diff(sub_dcmp)

def compare_directories_recursively(dir1, dir2):
    """
    Compares two directories recursively and prints a report of differences.
    """
    print(f"Comparing directories: '{dir1}' and '{dir2}'")
    dcmp = filecmp.dircmp(dir1, dir2)
    report_recursive_diff(dcmp)

if __name__ == '__main__':
    import sys
    dir1 = sys.argv[1]
    dir2 = sys.argv[2]
    compare_directories_recursively(dir1, dir2)

