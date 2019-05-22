#!/bin/bash
touch DelayvsThr.txt
cat /dev/null >| ./DelayvsThr.txt
for directory in `ls ./DataCase`; do
	echo $directory
	Throughput=`sed -n '166p' ./DataCase/$directory/WSOL*Throughput*.plt | awk '{printf("%.1f\n",$2)}'`
	Delay=`sed -n '166p' ./DataCase/$directory/WSOL*Delay*.plt | awk '{print $2}'`
	echo "$Throughput $Delay" >> ./DelayvsThr.txt
done
mv ./DelayvsThr.txt ./DataCase