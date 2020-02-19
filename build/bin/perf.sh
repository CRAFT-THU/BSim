#!/bin/bash

for i in 0
do
	#./standard_test 800 1000 0.7 0.5 0.6 0.3 6 >> fr_2_1.perf
	CUDA_VISIBLE_DEVICES=0,1 ./standard_multi_test 800 1000 0.7 0.5 0.6 0.3 6 >> fr_2_2.perf
	./standard_multi_test 800 1000 0.7 0.5 0.6 0.3 6 >> fr_2_4.perf

	#./standard_test 800 1000 0.7 0.9 0.6 0.2 6 >> fr_5_1.perf
	CUDA_VISIBLE_DEVICES=0,1 ./standard_multi_test 800 1000 0.7 0.9 0.6 0.2 6 >> fr_5_2.perf
	./standard_multi_test 800 1000 0.7 0.9 0.6 0.2 6 >> fr_5_4.perf

	#./standard_test 800 1000 1.3 1 2 1 50 >> fr_20_1.perf
	CUDA_VISIBLE_DEVICES=0,1 ./standard_multi_test 800 1000 1.3 1 2 1 50 >> fr_20_2.perf
	./standard_multi_test 800 1000 1.3 1 2 1 50 >> fr_20_4.perf
done
