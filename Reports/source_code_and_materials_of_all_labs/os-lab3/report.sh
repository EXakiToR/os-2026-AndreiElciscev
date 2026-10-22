#!/bin/bash
# usage: ./report.sh access.log

log=$1

if [ ! -f "$log" ]; then
    echo "no such file: $log" >&2
    exit 1
fi

total=$(wc -l < "$log")
fails=$(grep -c FAIL "$log")

echo "$total requests, $fails failed"

for user in user0 user1 user2 user3 user4 user5 user6
do
    count=$(grep FAIL "$log" | grep -c " $user ")
    echo "$user: $count failures"
done
