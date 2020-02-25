#!/usr/bin/env bash

# Exit immediately if any command below fails.
set -e

make clean > /dev/null
make > /dev/null

numbers=("fib(25) = 121393" "fib(30) = 1346269"	"fib(40) = 165580141" "fib(5) = 8" "fib(4) = 5" "fib(3) = 3" "fib(2) = 2" "fib(1) = 1")

cat inputLarge | ./fibs -n 4 > fibs.actual
cat input | ./fibs -n 2 ./test >> fibs.actual

success="Success" 

for i in "${numbers[@]}"; do
	if [[ $(grep "$i" fibs.actual -c) != "1" ]]; then
		success="Failed"
	fi
done

echo ">>> Testing fibs: ${success}"

./fauxgrep test ./test > fauxgrep.expected
./fauxgrep-mt -n 2 test ./test > fauxgrep.actual

if ! diff -u fauxgrep.expected fauxgrep.actual
then
  echo ">>> Testing fauxgrep: Failed"
  exitcode=1
else
  echo ">>> Testing fauxgrep: Success"
fi

./fhistogram ./test > fhistogram.expected &
./fhistogram-mt -n 2 ./test > fhistogram.actual &
wait

if ! diff -u <(tail -n 9 fhistogram.actual) <(tail -n 9 fhistogram.expected)
then
  echo ">>> Testing fhistogram: Failed"
  exitcode=1
else
  echo ">>> Testing fhistogram: Success"
fi