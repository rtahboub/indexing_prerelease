set terminal postscript eps enhanced color 24
#set title "RTree/CRTree Node Size"

set xlabel "Node Size in Bytes"
set ylabel "Avg. Time per Tick (s)"

set yrange [0:]

set output "out/eps/RTree/RTreeNodeSize.eps"
#set output "out/eps/RTree/RTreeNodeSizeLarge.png"

# plot "out/outfiles/RTree/RTreeNodeSizeAvg.txt" using 1:(($3+$5+$7)/100.0) with linespoints title "R-Tree" lw 7, \
#      "out/outfiles/RTree/CRTree8NodeSizeAvg.txt" using 1:(($3+$5+$7)/100.0) with linespoints title "CR-Tree, 1 Byte" lw 7, \
#      "out/outfiles/RTree/CRTree16NodeSizeAvg.txt" using 1:(($3+$5+$7)/100.0) with linespoints title "CR-Tree, 2 Bytes" lw 7, \
#      "out/outfiles/RTree/CRTree32NodeSizeAvg.txt" using 1:(($3+$5+$7)/100.0) with linespoints title "CR-Tree, 4 Bytes" lw 7
     
plot "out/outfiles/RTree/RTreeNodeSizeAvg.txt" using 1:(($3+$5+$7)/100.0):(($9+$11+$13)/100.0):(($15+$17+$19)/100.0) ls 1 lw 7 with errorbars notitle, \
     "" using 1:(($3+$5+$7)/100.0) with lines ls 1 lw 7 title "R-Tree", \
     "out/outfiles/RTree/CRTree8NodeSizeAvg.txt" using 1:(($3+$5+$7)/100.0):(($9+$11+$13)/100.0):(($15+$17+$19)/100.0) ls 2 lw 7 with errorbars notitle, \
     "" using 1:(($3+$5+$7)/100.0) with lines ls 2 lw 7 title "CR-Tree, 1 Byte", \
     "out/outfiles/RTree/CRTree16NodeSizeAvg.txt" using 1:(($3+$5+$7)/100.0):(($9+$11+$13)/100.0):(($15+$17+$19)/100.0) ls 3 lw 7 with errorbars notitle, \
     "" using 1:(($3+$5+$7)/100.0) with lines ls 3 lw 7 title "CR-Tree, 2 Bytes", \
     "out/outfiles/RTree/CRTree32NodeSizeAvg.txt" using 1:(($3+$5+$7)/100.0):(($9+$11+$13)/100.0):(($15+$17+$19)/100.0) ls 4 lw 7 with errorbars notitle, \
     "" using 1:(($3+$5+$7)/100.0) with lines ls 4 lw 7 title "CR-Tree, 4 Bytes"

# set output "out/eps/RTree/RTreeNodeSizeSmall.eps"

# plot "out/outfiles/RTree/RTreeNodeSizeAvg.txt" using 1:($3+$5+$7) with linespoints title "RTree" lw 7, \
#      "out/outfiles/RTree/CRTree8NodeSizeAvg.txt" using 1:($3+$5+$7) with linespoints title "CRTree8" lw 7, \
#      "out/outfiles/RTree/CRTree16NodeSizeAvg.txt" using 1:($3+$5+$7) with linespoints title "CRTree16" lw 7, \
#      "out/outfiles/RTree/CRTree32NodeSizeAvg.txt" using 1:($3+$5+$7) with linespoints title "CRTree32" lw 7

# set output "out/eps/RTree/RTreeNodeSizeLarge.eps"

# plot "out/outfiles/RTree/RTreeNodeSizeAvg.txt" using 1:($9+$11+$13) with linespoints title "RTree" lw 7, \
#      "out/outfiles/RTree/CRTree8NodeSizeAvg.txt" using 1:($9+$11+$13) with linespoints title "CRTree8" lw 7, \
#      "out/outfiles/RTree/CRTree16NodeSizeAvg.txt" using 1:($9+$11+$13) with linespoints title "CRTree16" lw 7, \
#      "out/outfiles/RTree/CRTree32NodeSizeAvg.txt" using 1:($9+$11+$13) with linespoints title "CRTree32" lw 7

# set output "out/eps/RTree/RTreeNodeSizeHuge.eps"

# plot "out/outfiles/RTree/RTreeNodeSizeAvg.txt" using 1:($15+$17+$19) with linespoints title "RTree" lw 7, \
#      "out/outfiles/RTree/CRTree8NodeSizeAvg.txt" using 1:($15+$17+$19) with linespoints title "CRTree8" lw 7, \
#      "out/outfiles/RTree/CRTree16NodeSizeAvg.txt" using 1:($15+$17+$19) with linespoints title "CRTree16" lw 7, \
#      "out/outfiles/RTree/CRTree32NodeSizeAvg.txt" using 1:($15+$17+$19) with linespoints title "CRTree32" lw 7

# set output "out/eps/RTree/RTreeNodeSizeGauss.eps"

# plot "out/outfiles/RTree/RTreeNodeSizeAvg.txt" using 1:($21+$23+$25) with linespoints title "RTree" lw 7, \
#      "out/outfiles/RTree/CRTree8NodeSizeAvg.txt" using 1:($21+$23+$25) with linespoints title "CRTree8" lw 7, \
#      "out/outfiles/RTree/CRTree16NodeSizeAvg.txt" using 1:($21+$23+$25) with linespoints title "CRTree16" lw 7, \
#      "out/outfiles/RTree/CRTree32NodeSizeAvg.txt" using 1:($21+$23+$25) with linespoints title "CRTree32" lw 7
     


