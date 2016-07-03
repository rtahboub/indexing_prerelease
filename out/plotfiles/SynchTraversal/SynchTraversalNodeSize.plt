set terminal postscript eps enhanced color 24

set xlabel "RTree Node Size"
set ylabel "Time (s)"


set yrange [0:]

set key left

#set title "Varying RTree Node Size for SynchTraversal"
set output "out/eps/SynchTraversal/SynchTraversalNodeSize.eps"

plot "out/outfiles/SynchTraversal/SynchTraversalNodeSizeAvg.txt" using 1:(($3+$5+$7)/100.0) with linespoints title "Nested Loops" lw 7, \
     "" using 1:(($9+$11+$13)/100.0) with linespoints title "Restricted Pruning" lw 7, \
     "" using 1:(($15+$17+$19)/100.0) with linespoints title "Plane-Sweep" lw 7


# set title "Varying RTree Node Size for SynchTraversal on Samll Trace"
# set output "out/eps/SynchTraversal/SynchTraversalNodeSizeSmall.eps"

# plot "out/outfiles/SynchTraversal/SynchTraversalNodeSizeAvg.txt" using 1:($3+$5+$7) with linespoints title "SynchTraversal" lw 5, \
#      "" using 1:($27+$29+$31) with linespoints title "SynchTraversalRes" lw 5, \
#      "" using 1:($51+$53+$55) with linespoints title "SynchTraversalSweep" lw 5


# set title "Varying RTree Node Size for SynchTraversal on Large Trace"
# set output "out/eps/SynchTraversal/SynchTraversalNodeSizeLarge.eps"
     
# plot "out/outfiles/SynchTraversal/SynchTraversalNodeSizeAvg.txt" using 1:($9+$11+$13) with linespoints title "SynchTraversal" lw 5, \
#      "" using 1:($33+$35+$37) with linespoints title "SynchTraversalRes" lw 5, \
#      "" using 1:($57+$59+$61) with linespoints title "SynchTraversalSweep" lw 5


# set title "Varying RTree Node Size for SynchTraversal on Huge Trace"
# set output "out/eps/SynchTraversal/SynchTraversalNodeSizeHuge.eps"
     
# plot "out/outfiles/SynchTraversal/SynchTraversalNodeSizeAvg.txt" using 1:($15+$17+$19) with linespoints title "SynchTraversal" lw 5, \
#      "" using 1:($39+$41+$43) with linespoints title "SynchTraversalRes" lw 5, \
#      "" using 1:($63+$65+$67) with linespoints title "SynchTraversalSweep" lw 5
     

# set title "Varying RTree Node Size for SynchTraversal on Gauss Trace"
# set output "out/eps/SynchTraversal/SynchTraversalNodeSizeGauss.eps"
     
# plot "out/outfiles/SynchTraversal/SynchTraversalNodeSizeAvg.txt" using 1:($21+$23+$25) with linespoints title "SynchTraversal" lw 5, \
#      "" using 1:($45+$47+$49) with linespoints title "SynchTraversalRes" lw 5, \
#      "" using 1:($69+$71+$73) with linespoints title "SynchTraversalSweep" lw 5
