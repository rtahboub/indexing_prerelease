set terminal postscript eps enhanced color 24

set output "out/eps/Fish/ScaleFish.eps"

#set title "Scaling the Number of Points"

set key left top

set xlabel "Num. Fish"
set ylabel "Avg. Time per Tick (s)"

set yrange [0:350]

set xtics   ("100K" 100000, "300K" 300000, "500K" 500000, "700K" 700000, "900K" 900000)

plot "out/outfiles/Fish/fishlarge.dat" using 1:(($3 + $5)/100.0) with linespoints title "R-Tree" lw 7,\
     "" using 1:(($9+$11)/100.0) with linespoints title "SynchTraversal" lw 7,\
     "" using 1:(($15+$17)/100.0) with linespoints title "TPR-Tree" lw 7
