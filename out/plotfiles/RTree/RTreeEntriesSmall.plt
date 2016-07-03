set terminal postscript eps enhanced color 

set title "RTree/CRTree Node Size"

set xlabel "Node Size in Bytes"
set ylabel "Time (s)"


set output "out/eps/RTree/RTreeNodeSizeSmall.eps"

plot "out/outfiles/RTree/RTreeNodeSizeAvg.txt" using 1:($3+$5+$7) with linespoints title "RTree" lw 3, \
     "out/outfiles/RTree/CRTree8NodeSizeAvg.txt" using 1:($3+$5+$7) with linespoints title "CRTree8" lw 3, \
     "out/outfiles/RTree/CRTree16NodeSizeAvg.txt" using 1:($3+$5+$7) with linespoints title "CRTree16" lw 3, \
     "out/outfiles/RTree/CRTree32NodeSizeAvg.txt" using 1:($3+$5+$7) with linespoints title "CRTree32" lw 3

set output "out/eps/RTree/RTreeNodeSizeLarge.eps"

plot "out/outfiles/RTree/RTreeNodeSizeAvg.txt" using 1:($9+$11+$13) with linespoints title "RTree" lw 3, \
     "out/outfiles/RTree/CRTree8NodeSizeAvg.txt" using 1:($9+$11+$13) with linespoints title "CRTree8" lw 3, \
     "out/outfiles/RTree/CRTree16NodeSizeAvg.txt" using 1:($9+$11+$13) with linespoints title "CRTree16" lw 3, \
     "out/outfiles/RTree/CRTree32NodeSizeAvg.txt" using 1:($9+$11+$13) with linespoints title "CRTree32" lw 3

set output "out/eps/RTree/RTreeNodeSizeHuge.eps"

plot "out/outfiles/RTree/RTreeNodeSizeAvg.txt" using 1:($15+$17+$19) with linespoints title "RTree" lw 3, \
     "out/outfiles/RTree/CRTree8NodeSizeAvg.txt" using 1:($15+$17+$19) with linespoints title "CRTree8" lw 3, \
     "out/outfiles/RTree/CRTree16NodeSizeAvg.txt" using 1:($15+$17+$19) with linespoints title "CRTree16" lw 3, \
     "out/outfiles/RTree/CRTree32NodeSizeAvg.txt" using 1:($15+$17+$19) with linespoints title "CRTree32" lw 3

set output "out/eps/RTree/RTreeNodeSizeGauss.eps"

plot "out/outfiles/RTree/RTreeNodeSizeAvg.txt" using 1:($21+$23+$25) with linespoints title "RTree" lw 3, \
     "out/outfiles/RTree/CRTree8NodeSizeAvg.txt" using 1:($21+$23+$25) with linespoints title "CRTree8" lw 3, \
     "out/outfiles/RTree/CRTree16NodeSizeAvg.txt" using 1:($21+$23+$25) with linespoints title "CRTree16" lw 3, \
     "out/outfiles/RTree/CRTree32NodeSizeAvg.txt" using 1:($21+$23+$25) with linespoints title "CRTree32" lw 3
     


