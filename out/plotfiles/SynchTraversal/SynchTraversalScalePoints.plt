set terminal postscript eps enhanced color 24

set output "out/eps/SynchTraversal/SynchTraversalScalePoints.eps"

set title "Scaling the number of Points"

set key left top

set xlabel "Num. of Points"
set ylabel "Time (s)"

plot "out/outfiles/SynchTraversal/SynchTraversalScalePointsAvg.txt" using 1:($3+$5+$7) with linespoints title "SynchTraversal" lw 5,\
     "" using 1:($9+$11+$13) with linespoints title "SynchTraversalRes" lw 5, \
     "" using 1:($15+$17+$19) with linespoints title "SynchTraversalSweep" lw 5