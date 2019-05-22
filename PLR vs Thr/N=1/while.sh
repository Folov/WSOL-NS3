#!/bin/bash

arr=WSOL_2n
time_interval=0.00012;
mkdir DataCase
for (( i = 0; i < 10; i++ )); do
	./waf --run "scratch/my$arr --time_interval=$time_interval"
	mkdir ./DataCase/$time_interval
	./GnuPlot.sh
	mv $arr* ./DataCase/$time_interval/
	time_interval=$(printf "%.6f" `echo "scale=6; $time_interval *1.2" | bc`)
done

touch ThroughputvsLPR.txt
cat /dev/null >| ./ThroughputvsLPR.txt
for directory in `ls ./DataCase`; do
	echo $directory
	Throughput=`sed -n '166p' ./DataCase/$directory/WSOL*Throughput*.plt | awk '{printf("%.1f\n",$2)}'`
	LostPackets=`sed -n '166p' ./DataCase/$directory/WSOL*LostPackets*.plt | awk '{print $2}'`
	echo "$Throughput $LostPackets" >> ./ThroughputvsLPR.txt
done
mv ./ThroughputvsLPR.txt ./DataCase