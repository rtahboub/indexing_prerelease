set terminal postscript eps enhanced color 24

set output "out/eps/MovingIndex/MovingIndexSmall.eps"

#set title "Scaling the Number of Points"

set key left top

set xlabel "Fraction of points issuing queries"
set ylabel "Time (s)"

set xtics   ("0.01" 0.01, "0.05" 0.05, "0.09" 0.09)

plot "out/outfiles/MovingIndex/MovingIndexSmall.txt" using 1:($4+$6+$8) with linespoints title "RTree" lw 7,\
     "" using 1:($10+$12+$14) with linespoints title "TPRTree" lw 7,\
     "" using 1:($16+$18+$20) with linespoints title "STRIPES" lw 7
