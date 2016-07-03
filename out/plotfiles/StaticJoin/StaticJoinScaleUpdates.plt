set terminal postscript eps enhanced color 24

set output "out/eps/StaticJoin/StaticJoinScaleUpdates.eps"

# set title "Scaling the Fraction of Updates"

set key left top

set xlabel "Update Fraction"
set ylabel "Time (s)"

     
# plot "out/outfiles/StaticJoin/StaticJoinScaleUpdatesAvg.txt" using 1:($3+$5+$7) with linespoints title "ListSweep" lw 7,\
#      "" using 1:($9+$11+$13) with linespoints title "StripedSweep128" lw 7,\
#      "" using 1:($15+$17+$19) with linespoints title "ForwardPBSM" lw 7,\
#      "" using 1:($21+$23+$25) with linespoints title "SynchTraversal" lw 7


plot "out/outfiles/StaticJoin/StaticJoinScaleUpdatesAvg.txt" using 1:(($3+$5+$7)/100.0) with linespoints title "List-Sweep" lw 7,\
     "" using 1:(($9+$11+$13)/100.0) with linespoints title "Striped-Sweep 128" lw 7,\
     "" using 1:(($15+$17+$19)/100.0) with linespoints title "Forward-PBSM" lw 7,\
     "" using 1:(($21+$23+$25)/100.0) with linespoints title "Synch. Traversal" lw 7
     