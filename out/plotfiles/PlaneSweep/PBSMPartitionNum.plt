set terminal postscript eps enhanced color 24

set xlabel "Partitions per Side"
set ylabel "Avg. Time per Tick (s)"
set yrange [0:]

set output "out/eps/PlaneSweep/PBSMPartitionNum.eps"

plot "out/outfiles/PlaneSweep/PBSMPartitionNumAvg.txt" using 1:(($3+$5+$7)/100.0):(($9+$11+$13)/100.0):(($15+$17+$19)/100.0) with errorbars ls 1 lw 7 notitle,\
     "" using 1:(($3+$5+$7)/100.0) notitle with lines ls 1 lw 7

# set title "Varying Num. Partitions for PBSM on Small Trace"
# set output "out/eps/PlaneSweep/PBSMPartitionNumSmall.eps"

# plot "out/outfiles/PlaneSweep/PBSMPartitionNumAvg.txt" using 1:($3+$5+$7) with linespoints title "ForwardPBSM" lw 5, \
#      "" using 1:($27+$29+$31) with linespoints title "GenericPBSMStriped128" lw 5


# set title "Varying Num. Partitions for PBSM on Large Trace"
# set output "out/eps/PlaneSweep/PBSMPartitionNumLarge.eps"


# plot "out/outfiles/PlaneSweep/PBSMPartitionNumAvg.txt" using 1:($9+$11+$13) with linespoints title "ForwardPBSM" lw 5, \
#      "" using 1:($33+$35+$37) with linespoints title "GenericPBSMStriped128" lw 5




# set title "Varying Num. Partitions for PBSM on Huge Trace"
# set output "out/eps/PlaneSweep/PBSMPartitionNumHuge.eps"


# plot "out/outfiles/PlaneSweep/PBSMPartitionNumAvg.txt" using 1:($15+$17+$19) with linespoints title "ForwardPBSM" lw 5, \
#      "" using 1:($39+$41+$43) with linespoints title "GenericPBSMStriped128" lw 5



# set title "Varying Num. Partitions for PBSM on Gauss Trace"
# set output "out/eps/PlaneSweep/PBSMPartitionNumGauss.eps"


# plot "out/outfiles/PlaneSweep/PBSMPartitionNumAvg.txt" using 1:($21+$23+$25) with linespoints title "ForwardPBSM" lw 5, \
#      "" using 1:($45+$47+$49) with linespoints title "GenericPBSMStriped128" lw 5
     










     










     