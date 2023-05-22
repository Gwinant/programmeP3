#!/bin/bash

make timer

echo ""> ./test_perf/MaxRAMoccupied.txt

timing(){
	
	
	./main -n "$1" graph/graph10.bin
	
	array=(graph/graph10.bin)
	echo "***************** "$1" threads *****************">> ./test_perf/MaxRAMoccupied.txt
	for i in ${array[*]}
	do
	echo Time : "$i" with "$1" threads
	echo "$i">> ./test_perf/MaxRAMoccupied.txt
	command time -o ./test_perf/MaxRAMoccupied.txt -a -f "Max portion of RAM occupied [Kbytes] \t %M\n" ./main -n "$1" "$i"

	done
	
}

timing 1

./timer

make clean