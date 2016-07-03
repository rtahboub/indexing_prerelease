set terminal postscript eps enhanced color 24

set output "out/eps/StaticIndex/StaticIndexScaleUpdates.eps"

set title "Scaling the fraction of Updates"

set key left top

set xlabel "Fraction of points being updated"
set ylabel "Time (s)"

# plot "out/outfiles/StaticIndex/StaticIndexScaleUpdatesAvg.txt" using 1:($3+$5+$7) with linespoints title "BinarySearch2D" lw 5,\
#      "" using 1:($9+$11+$13) with linespoints title "RTree2D" lw 5,\
#      "" using 1:($15+$17+$19) with linespoints title "CRTree2D" lw 5,\
#      "" using 1:($27+$29+$31) with linespoints title "LinearizedKDTrie" lw 5, \
#      "" using 1:($33+$35+$37) with linespoints title "SimpleGrid" lw 5



plot "out/outfiles/StaticIndex/StaticIndexScaleUpdatesAvg.txt" using 1:(($3+$5+$7)/100.0) with linespoints title "BinarySearch2D" lw 7,\
     "" using 1:(($9+$11+$13)/100.0) with linespoints title "RTree2D" lw 7,\
     "" using 1:(($15+$17+$19)/100.0) with linespoints title "CRTree2D" lw 7,\
     "" using 1:(($21+$23+$25)/100.0) with linespoints title "LinearizedKDTrie" lw 7, \
     "" using 1:(($27+$29+$31)/100.0) with linespoints title "SimpleGrid" lw 7

     #     "" using 1:($21+$23+$25) with linespoints title "OrthogonalRangeTree" lw 5,\