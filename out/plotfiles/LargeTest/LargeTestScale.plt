set terminal postscript eps enhanced color 24

set output "out/eps/LargeTest/LargeTestScale.eps"

set key left top

set xlabel "Num. of Points"
set ylabel "Time (s)"

set xtics   ("100K" 100000, "300K" 300000, "500K" 500000, "700K" 700000, "900K" 900000)

plot "out/outfiles/LargeTest/LargeTestScaleAvg.txt" using 1:($3+$5+$7) with linespoints title "RTree2D" lw 7,\
     "" using 1:($9+$11+$13) with linespoints title "CRTree2D" lw 7,\
     "" using 1:($15+$17+$19) with linespoints title "StripedSweep128" lw 7,\
     "" using 1:($21+$23+$25) with linespoints title "SynchTraversal2D" lw 7

