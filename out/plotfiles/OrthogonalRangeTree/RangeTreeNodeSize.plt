set terminal postscript eps enhanced color 24

set output "out/eps/OrthogonalRangeTree/RangeTreeNodeSize.eps"

set title "Varying Node Size in Orthogonal Range Tree"

set xlabel "Num. Entries per Node"
set ylabel "Time (s)"

set output "out/eps/OrthogonalRangeTree/RangeTreeNodeSizeSmall.eps"

plot "out/outfiles/OrthogonalRangeTree/RangeTreeNodeSizeAvg.txt" using 1:($3+$5+$7) with linespoints title "Small Trace" lw 5

set output "out/eps/OrthogonalRangeTree/RangeTreeNodeSizeLarge.eps"

# plot "out/outfiles/OrthogonalRangeTree/RangeTreeNodeSizeAvg.txt" using 1:($9+$11+$13) with linespoints title "Large Trace" lw 5

# set output "out/eps/OrthogonalRangeTree/RangeTreeNodeSizeHuge.eps"

# plot "out/outfiles/OrthogonalRangeTree/RangeTreeNodeSizeAvg.txt" using 1:($15+$17+$19) with linespoints title "Large Trace" lw 5

set output "out/eps/OrthogonalRangeTree/RangeTreeNodeSizeGauss.eps"

plot "out/outfiles/OrthogonalRangeTree/RangeTreeNodeSizeAvg.txt" using 1:($15+$17+$19) with linespoints title "Gauss Trace" lw 5

