#!/bin/bash

# $1 - Which directory and it's sub directory to perform search
# $2 - Keyword to search

FILES=$(grep "^TARGET *=" `find $1 -name "Makefile" -o -name "*.make"` | grep -E "$2" | awk -F ':' '{ print $1}')
for FILE in $FILES; do
  echo $(dirname $FILE)
done