#!/bin/bash

# $1 - Which directory and it's sub directory to perform search
# $2 - Search keyword

STR=$(grep "^TARGET" `find $1 -name "Makefile" -o -name "*.make"` | grep -E "$2")
FILE=$(echo -e $STR | awk -F ':' '{ print $1}')
DIR=$(dirname $FILE)

echo $DIR