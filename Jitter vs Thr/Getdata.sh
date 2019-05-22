#!/bin/bash
touch JittervsThr.txt
cat /dev/null >| ./JittervsThr.txt
for directory in `ls ./DataCase`; do
	echo $directory
	Throughput=`sed -n '166p' ./DataCase/$directory/WSOL*Throughput*.plt | awk '{printf("%.1f\n",$2)}'`
	Delay=`sed -n '166p' ./DataCase/$directory/WSOL*Jitter*.plt | awk '{print $2}'`
	echo "$Throughput $Delay" >> ./JittervsThr.txt
done
mv ./JittervsThr.txt ./DataCase