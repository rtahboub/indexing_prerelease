set terminal postscript eps enhanced color "Arial" 24

set output "out/eps/GrantCrossover.eps"

set title "Scaling the number of points"

set key left top
set xtic (10000, 30000, 50000, 70000, 90000)
set xlabel "Num. of Points"
set ylabel "Time (s)"

plot "out/outfiles/GrantCrossover.txt" using 1:($3+$5+$7) title "Binary Search" with linespoints ls 11,\
      "" using 1:($9+$11+$13) title "RTree" with linespoints ls 22,\
      "" using 1:($15+$17+$19) title "Synch. Traversal" with linespoints ls 32,\
      "" using 1:($21+$23+$25) title "Plane Sweep" with linespoints ls 35,\
      "" using 1:($27+$29+$31) title "PBSM" with linespoints ls 30,\
      "" using 1:($33+$35+$37) title "TPRTree" with linespoints ls 10