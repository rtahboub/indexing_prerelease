set terminal postscript eps enhanced color 24

set output "out/eps/Network/ScaleNetwork.eps"

#set title "Scaling the number of Points"

set key left top

set xlabel "Num. of Fish"
set ylabel "Time (s)"

set yrange [0:]

# set xtics   ("10K" 10000, "30K" 30000, "50K" 50000, "70K" 70000, "90K" 90000)

plot "out/outfiles/Fish/ScaleFishAvg.txt" using 1:($3+$5+$7) with linespoints title "CRTree2D" lw 5,\
     "" using 1:($9+$11+$13) with linespoints title "StripedSweep128" lw 7,\
     "" using 1:($15+$17+$19) with linespoints title "SynchTraversal" lw 7,\
     "" using 1:($21+$23+$25) with linespoints title "TPRTree2D" lw 7,\
     "" using 1:($27+$29+$31) with linespoints title "OptimizedStripes" lw 7

# plot "out/outfiles/PlaneSweep/PlaneSweepScalePointsAvg.txt" using 1:($3+$5+$7) with linespoints title "ForwardPlaneSweep" lw 5,\
#      "" using 1:($9+$11+$13) with linespoints title "ListSweep" lw 5,\
#      "" using 1:($15+$17+$19) with linespoints title "StripedSweep8" lw 5,\
#      "" using 1:($21+$23+$25) with linespoints title "StripedSweep16" lw 5,\
#      "" using 1:($27+$29+$31) with linespoints title "StripedSweep32" lw 5,\
#      "" using 1:($33+$35+$37) with linespoints title "StripedSweep64" lw 5,\
#      "" using 1:($39+$41+$43) with linespoints title "StripedSweep128" lw 5,\
#      "" using 1:($45+$47+$49) with linespoints title "ForwardPBSM" lw 5,\
#      "" using 1:($51+$53+$55) with linespoints title "ListPBSM" lw 5
     

