#!/bin/bash
# 修正格式：fix: (ui)xxx → fix(ui): xxx
git log --pretty=format:"%s" $1..HEAD | \
  sed -E 's/^([^:]+): \(([^)]+)\)(.*)/\1(\2):\3/'
