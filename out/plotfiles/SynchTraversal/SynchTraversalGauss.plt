set terminal postscript eps enhanced color 24

set output "out/eps/SynchTraversal/SynchTraversalGauss.eps"

set title "Scaling the number of HotSpots"

set key left top

set logscale x

set xlabel "Num. of HotSpots, logscale"
set ylabel "Time (s)"

plot "out/outfiles/SynchTraversal/SynchTraversalGaussAvg.txt" using 1:($3+$5+$7) with linespoints title "SynchTraversal" lw 5,\
     "" using 1:($9+$11+$13) with linespoints title "SynchTraversalRes" lw 5, \
     "" using 1:($15+$17+$19) with linespoints title "SynchTraversalSweep" lw 5