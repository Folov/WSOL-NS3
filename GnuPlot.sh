#!/bin/bash

if [ -e "./WSOL_2nThroughputVSTime.plt" ]; then
	gnuplot WSOL_2nThroughputVSTime.plt
	gnuplot WSOL_2nDelayVSTime.plt
	gnuplot WSOL_2nJitterVSTime.plt
	gnuplot WSOL_2nLostPacketsVSTime.plt
fi

if [[ -e "./WSOL_3nThroughputVSTime.plt" ]]; then
	gnuplot WSOL_3nThroughputVSTime.plt
	gnuplot WSOL_3nDelayVSTime.plt
	gnuplot WSOL_3nJitterVSTime.plt
	gnuplot WSOL_3nLostPacketsVSTime.plt
fi

if [[ -e "./WSOL_4nThroughputVSTime.plt" ]]; then
	gnuplot WSOL_4nThroughputVSTime.plt
	gnuplot WSOL_4nDelayVSTime.plt
	gnuplot WSOL_4nJitterVSTime.plt
	gnuplot WSOL_4nLostPacketsVSTime.plt
fi

if [[ -e "./WSOL_5nThroughputVSTime.plt" ]]; then
	gnuplot WSOL_5nThroughputVSTime.plt
	gnuplot WSOL_5nDelayVSTime.plt
	gnuplot WSOL_5nJitterVSTime.plt
	gnuplot WSOL_5nLostPacketsVSTime.plt
fi

exit 0