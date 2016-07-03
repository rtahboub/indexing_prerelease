set terminal postscript eps enhanced color 24

set output "out/eps/MovingIndex/MovingIndexScaleUpdates.eps"

#set title "Scaling the fraction of Updates"

set key at 0.9,4.2

set xlabel "Fraction of points issuing updates"
set ylabel "Avg. Time per Tick (s)"

set xtics   ("0.1" 0.1, "0.3" 0.3, "0.5" 0.5, "0.7" 0.7, "0.9" 0.9)

# plot "out/outfiles/MovingIndex/MovingIndexScaleUpdatesAvg.txt" using 1:($3+$5+$7) with linespoints title "TPR-Tree" lw 7,\
#      "" using 1:($9+$11+$13) with linespoints title "TPR-Tree" lw 7

# plot "out/outfiles/MovingIndex/MovingIndexScaleUpdatesAvg.txt" using 1:(($3+$5+$7)/100.0) with linespoints title "TPR-Tree" lw 7,\
#      "" using 1:(($9+$11+$13)/100.0) with linespoints title "STRIPES" lw 7


plot "out/outfiles/MovingIndex/MovingIndexScaleUpdatesAvg.txt" using 1:(($3+$5+$7)/100.0):(($9+$11+$13)/100.0):(($15+$17+$19)/100.0) with errorbars ls 1 lw 7 title "STRIPES", \
     "" using 1:(($3+$5+$7)/100.0) notitle with lines ls 1 lw 7, \
     "" using 1:(($21+$23+$25)/100.0):(($27+$29+$31)/100.0):(($33+$35+$37)/100.0) with errorbars ls 2 lw 7 title "TPR-Tree", \
     "" using 1:(($21+$23+$25)/100.0) notitle with lines ls 2 lw 7
     