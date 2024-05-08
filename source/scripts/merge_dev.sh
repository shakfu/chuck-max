#!/usr/bin/env sh

git switch main
git merge dev
git push
git switch dev
