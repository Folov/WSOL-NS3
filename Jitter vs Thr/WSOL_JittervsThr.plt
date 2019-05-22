set terminal png
set log x 2
set output "WSOL_JittervsThr.png"
set title "Jitter vs Throughput"
set xlabel "Throughput (Mbps)"
set ylabel "Jitter (ms)"
set yrange [0:1]
set key box
# set key center at 5.55,335 
plot "WSOL_JittervsThr.dat" using 1:2 title " N=1" lc rgb "#0E0E0E" lw 1 pt 7 ps 1, \
						'' using 1:3 title " N=2" lc rgb "#007850" lw 1 pt 2 ps 1, \
						'' using 1:4 title " N=3" lc rgb "#73082F" lw 1 pt 16 ps 1, \
						'' using 1:5 title " N=4" lc rgb "#AD6E07" lw 1 pt 11 ps 1, \
						'' using 1:6 title " N=5" lc rgb "#204792" lw 1 pt 1 ps 1
