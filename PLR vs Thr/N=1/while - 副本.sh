#!/bin/bash

time_interval=0.0003;
for (( i = 0; i < 5; i++ )); do
	./waf --run "scratch/myWSOL_2n --time_interval=$time_interval"
	mkdir $time_interval
	./GnuPlot.sh
	mv WSOL_2n* $time_interval/
	time_interval=$(printf "%.6f" `echo "scale=6; $time_interval *1.5" | bc`)
done