set terminal postscript eps enhanced color 

set output "out/eps/PlaneSweepScaleUpdatesTrial1Tick1Results.eps"

set title "Scaling the fraction of updates."

set key left top

set xlabel "Fraction of points updated each tick."
set ylabel "Num. Join Results"

#plot "out/outfiles/PlaneSweepScaleUpdatesTrial1Tick1.txt" using 1:($3+$5+$7) with linespoints title "RTree Tick Time" lw 3
plot "out/outfiles/PlaneSweepScaleUpdatesTrial1Tick1.txt" using 1:9 with linespoints title "NumResults" lw 3



