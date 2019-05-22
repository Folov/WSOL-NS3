#!/bin/bash

arr=WSOL_3n
time_interval=0.00036;
mkdir DataCase
for (( i = 0; i < 40; i++ )); do
	./waf --run "scratch/my$arr --time_interval=$time_interval"
	mkdir ./DataCase/$time_interval
	./GnuPlot.sh
	mv $arr* ./DataCase/$time_interval/
	time_interval=$(printf "%.6f" `echo "scale=6; $time_interval *1.12" | bc`)
done

touch DelayvsThr.txt
cat /dev/null >| ./DelayvsThr.txt
for directory in `ls ./DataCase`; do
	echo $directory
	Throughput=`sed -n '166p' ./DataCase/$directory/WSOL*Throughput*.plt | awk '{printf("%.1f\n",$2)}'`
	Delay=`sed -n '166p' ./DataCase/$directory/WSOL*Delay*.plt | awk '{print $2}'`
	echo "$Throughput $Delay" >> ./DelayvsThr.txt
done
mv ./DelayvsThr.txt ./DataCase