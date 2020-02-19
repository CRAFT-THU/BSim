#!/bin/bash
#for i in 12 16
for i in 120 160 200 240 280
do
	echo $i
	for j in 0 1 2
	do
		./standard_test $i 1000 0.7 0.5 0.6 0.3 6  >> $i.perf
	done
	for j in 0 1 2
	do
		/home/yhx/bsim/build/bin/standard_test $i 1000 0.7 0.5 0.6 0.3 6  >> $i.lol.perf
	done
done
