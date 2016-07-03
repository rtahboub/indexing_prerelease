set terminal postscript eps enhanced color 24

set output "out/eps/PlaneSweep/PlaneSweepScaleQueries.eps"

set title "Scaling the fraction of queries."

set key left top

set xlabel "Fraction of points querying each tick."
set ylabel "Time (s)"


plot "out/outfiles/PlaneSweep/PlaneSweepScaleQueriesAvg.txt" using 1:($3+$5+$7) with linespoints title "ForwardPlaneSweep" lw 5,\
     "" using 1:($9+$11+$13) with linespoints title "ListSweep" lw 5,\
     "" using 1:($15+$17+$19) with linespoints title "StripedSweep8" lw 5,\
     "" using 1:($21+$23+$25) with linespoints title "StripedSweep16" lw 5,\
     "" using 1:($27+$29+$31) with linespoints title "StripedSweep32" lw 5,\
     "" using 1:($33+$35+$37) with linespoints title "StripedSweep64" lw 5,\
     "" using 1:($39+$41+$43) with linespoints title "StripedSweep128" lw 5,\
     "" using 1:($45+$47+$49) with linespoints title "ForwardPBSM" lw 5,\
     "" using 1:($51+$53+$55) with linespoints title "ListPBSM" lw 5
