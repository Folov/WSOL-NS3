set terminal png
set output "WSOL_DelayVSTime.png"
set title "Mean Delay vs Time"
set xlabel "Time (s)"
set ylabel "Mean Delay (ms)"
set key box
set key center at 5.55,9.9
plot "WSOL_DelayVSTime.dat" using 1:2 title " N=1" w lp lc rgb "#0E0E0E" lw 1 pt 7 ps 1, \
					     '' using 1:3 title " N=2" w lp lc rgb "#007850" lw 1 pt 2 ps 1, \
					     '' using 1:4 title " N=3" w lp lc rgb "#73082F" lw 1 pt 16 ps 1, \
					     '' using 1:5 title " N=4" w lp lc rgb "#AD6E07" lw 1 pt 11 ps 1, \
					     '' using 1:6 title " N=5" w lp lc rgb "#204792" lw 1 pt 1 ps 1
