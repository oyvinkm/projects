#!/bin/bash
echo
echo "********************************"
echo "          fibs                  "
echo "********************************"

for i in {1..6}; do
	sync && echo 3 > /proc/sys/vm/drop_caches
	echo
	echo "n = ${i}"
    time -p cat inputTiming | ./fibs -n "${i}" > /dev/null
	echo
done

echo "********************************"
echo "          fauxgrep            "
echo "********************************"
echo
echo "Files in directory:"
find ../../../../ -type f | wc -l
echo
echo "original"
sync && echo 3 > /proc/sys/vm/drop_caches

time -p ./fauxgrep test ../../../../ > /dev/null

for i in {1..6}; do
	sync && echo 3 > /proc/sys/vm/drop_caches
	echo
	echo "n = ${i}"
	time -p ./fauxgrep-mt -n "${i}" test ../../../../ > /dev/null
	echo
done

echo "********************************"
echo "          fhistogram            "
echo "********************************"
echo
echo "Files in directory:"
find ../../../ -type f | wc -l
echo
echo "original"
sync && echo 3 > /proc/sys/vm/drop_caches
time -p ./fhistogram -n ../../../
for i in {1..6}; do
	echo
	echo "n = ${i}"
	sync && echo 3 > /proc/sys/vm/drop_caches
	time -p ./fhistogram-mt -n "${i}" ../../../
	echo
done

