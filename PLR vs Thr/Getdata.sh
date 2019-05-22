#!/bin/bash
touch GetData.txt
cat /dev/null >| ./GetData.txt
for directory in `ls ./DataCase`; do
	echo $directory
	Throughput=`sed -n '166p' ./DataCase/$directory/WSOL*Throughput*.plt | awk '{printf("%.1f\n",$2)}'`
	LostPackets=`sed -n '166p' ./DataCase/$directory/WSOL*LostPackets*.plt | awk '{print $2}'`
	echo "$Throughput $LostPackets" >> ./GetData.txt
done