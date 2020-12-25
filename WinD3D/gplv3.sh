#!/bin/bash

for i in $(ls | egrep '\.cpp$|\.h$'); do
  year=$(git log --follow --diff-filter=A $i | grep Date | tail -1 | awk '{ print $6 }')
  echo "Adding GPL3 header w/ year $year to $i"
  cat ../lic.txt |sed "s|THEDATE|$year|" | cat - $i > /tmp/temp && mv /tmp/temp $i
done