set terminal postscript eps enhanced color 24

set xlabel "Gen. Cycle (ticks)"
set ylabel "Time (s)"

set key top left

set yrange [0:]

set output "out/eps/ContJoin/ContJoinHorizon.eps"

plot "out/outfiles/ContJoin/ContJoinHorizonAvg.txt" using 1:(($3+$5+$7)/100.0) with linespoints notitle lw 7
